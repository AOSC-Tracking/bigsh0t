/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>

#include "ImageProcessing.hpp"
#include "frei0r_math.h"
#include "Matrix.hpp"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
#define DEG2RADF(x) ((x) * M_PI / 180.0)

uint32_t sampleNearestNeighbor (const uint32_t* frame, double x, double y, int width, int height) {
    return frame[((int) y) * width + ((int) x)];
}

uint32_t lerp(uint32_t a, uint32_t b, int x) {
    int c0a = (a >> 24) & 0xff;
    int c1a = (a >> 16) & 0xff;
    int c2a = (a >>  8) & 0xff;
    int c3a = (a      ) & 0xff;

    int c0b = (b >> 24) & 0xff;
    int c1b = (b >> 16) & 0xff;
    int c2b = (b >>  8) & 0xff;
    int c3b = (b      ) & 0xff;

    int t = 0;

    int c0c = INT_BLEND(c0b, c0a, x, t);
    int c1c = INT_BLEND(c1b, c1a, x, t);
    int c2c = INT_BLEND(c2b, c2a, x, t);
    int c3c = INT_BLEND(c3b, c3a, x, t);

    return (c0c << 24) | (c1c << 16) | (c2c << 8) | c3c;
}

uint32_t sampleBilinear (const uint32_t* frame, double x, double y, int width, int height) {
    int ix0 = (int) x;
    int iy0 = (int) y;
    int ix1 = ix0 + 1;
    int iy1 = iy0 + 1;
    int ax = (int) ((x - ix0) * 256);
    int ay = (int) ((y - iy0) * 256);

    if (ix0 < 0 || iy0 < 0 || ix0 > width - 1 || iy0 > height - 1) {
        return 0;
    }
    if (ix1 >= width) {
        ix1 = width - 1;
    }
    if (iy1 >= height) {
        iy1 = height - 1;
    }

    uint32_t a = frame[iy0 * width + ix0];
    uint32_t b = frame[iy0 * width + ix1];
    uint32_t c = frame[iy1 * width + ix0];
    uint32_t d = frame[iy1 * width + ix1];
    uint32_t t = 0;

    uint32_t e = lerp(a, b, ax);
    uint32_t f = lerp(c, d, ax);
    return lerp(e, f, ay);
}

uint32_t sampleBilinearWrappedClamped (const uint32_t* frame, double x, double y, int width, int height) {
    if (x < 0) {
        x += width;
    }
    if (x >= width) {
        x -= width;
    }
    if (y < 0) {
        y = 0;
    }
    if (y >= height) {
        y = height;
    }

    int ix0 = (int) x;
    int iy0 = (int) y;
    int ix1 = ix0 + 1;
    int iy1 = iy0 + 1;
    int ax = (int) ((x - ix0) * 256);
    int ay = (int) ((y - iy0) * 256);


    if (ix1 >= width) {
        ix1 -= width;
    }
    if (iy1 >= height) {
        iy1 = height - 1;
    }

    uint32_t a = frame[iy0 * width + ix0];
    uint32_t b = frame[iy0 * width + ix1];
    uint32_t c = frame[iy1 * width + ix0];
    uint32_t d = frame[iy1 * width + ix1];

    uint32_t e = lerp(a, b, ax);
    uint32_t f = lerp(c, d, ax);
    return lerp(e, f, ay);
}

template<int interpolation>
void apply_360_map_tmpl(uint32_t* out, uint32_t* ibuf1, float* map, int width, int height, int start_scanline, int num_scanlines) {
    for (int yi = start_scanline; yi < start_scanline + num_scanlines; yi++) {
        for (int xi = 0; xi < width; xi++) {
            int idx = yi * width + xi;
            int midx = 2 * idx;
            float xt = map[midx];
            float yt = map[midx + 1];
            
            if (xt < 0) {
                out[idx] = 0;
                continue;
            }

            uint32_t pixel;
            switch(interpolation) {
                case Interpolation::NONE:
                    pixel = sampleNearestNeighbor(ibuf1, xt, yt, width, height);
                    break;
                case Interpolation::BILINEAR:
                    pixel = sampleBilinearWrappedClamped(ibuf1, xt, yt, width, height);
                    break;
            }
            out[idx] = pixel;
        }
    }
}

void apply_360_map(uint32_t* out, uint32_t* ibuf1, float* map, int width, int height, int start_scanline, int num_scanlines, int interpolation) {
    switch(interpolation) {
        case Interpolation::NONE:
            apply_360_map_tmpl<Interpolation::NONE>(out, ibuf1, map, width, height, start_scanline, num_scanlines);
            break;
        case Interpolation::BILINEAR:
            apply_360_map_tmpl<Interpolation::BILINEAR>(out, ibuf1, map, width, height, start_scanline, num_scanlines);
            break;
    }
}

template<int interpolation>
void transform_360_tmpl(uint32_t* out, uint32_t* ibuf1, int width, int height, int start_scanline, int num_scanlines, const Matrix3& xform) {

    int w = width;
    int h = height;
    
    int xi, yi;
    double xt, yt;

    Vector3 ray;
    Vector3 ray2;

    for (yi = start_scanline; yi < start_scanline + num_scanlines; yi++) {
        double phi = M_PI * ((double) yi - h / 2) / h;
        double sin_phi = sin(phi);
        double cos_phi = cos(phi);
        for (xi = 0; xi < w; xi++) {
            double theta = 2 * M_PI * ((double) xi - w / 2) / w;

            ray[0] = cos(theta) * cos_phi;
            ray[1] = sin(theta) * cos_phi;
            ray[2] = sin_phi;

            mulM3V3(xform, ray, ray2);

            double theta_out = atan2 (ray2[1], ray2[0]);
            double dxy = sqrt(ray2[0] * ray2[0] + ray2[1] * ray2[1]);
            double phi_out = atan2 (ray2[2], dxy);

            xt = w / 2 + (w / 2) * theta_out / M_PI;
            yt = h / 2 + (h / 2) * phi_out / (M_PI / 2);

            if (xt < 0) {
                xt += w;
            }
            if (xt >= w) {
                xt -= w;
            }

            if (yt < 0) {
                yt = 0;
            }
            if (yt > h - 1) {
                yt = h - 1;
            }

            /* interpolate */
            uint32_t pixel;
            switch(interpolation) {
                case Interpolation::NONE:
                    pixel = sampleNearestNeighbor(ibuf1, xt, yt, width, height);
                    break;
                case Interpolation::BILINEAR:
                    pixel = sampleBilinearWrappedClamped(ibuf1, xt, yt, width, height);
                    break;
            }
            out[((int) yi) * width + ((int) xi)] = pixel;
        }
    }
}

void transform_360(uint32_t* out, uint32_t* ibuf1, int width, int height, int start_scanline, int num_scanlines, double yaw, double pitch, double roll, int interpolation) {
    double yawR = DEG2RADF(yaw);
    double pitchR = DEG2RADF(pitch);
    double rollR = DEG2RADF(roll);

    Matrix3 xform;

    xform.identity();
    rotateX(xform, rollR);
    rotateY(xform, pitchR);
    rotateZ(xform, yawR);

    switch(interpolation) {
        case Interpolation::NONE:
            transform_360_tmpl<Interpolation::NONE>(out, ibuf1, width, height, start_scanline, num_scanlines, xform);
            break;
        case Interpolation::BILINEAR:
            transform_360_tmpl<Interpolation::BILINEAR>(out, ibuf1, width, height, start_scanline, num_scanlines, xform);
            break;
    }
}

void transform_360(uint32_t* out, uint32_t* ibuf1, int width, int height, int start_scanline, int num_scanlines, const Matrix3& xform, int interpolation) {
    switch(interpolation) {
        case Interpolation::NONE:
            transform_360_tmpl<Interpolation::NONE>(out, ibuf1, width, height, start_scanline, num_scanlines, xform);
            break;
        case Interpolation::BILINEAR:
            transform_360_tmpl<Interpolation::BILINEAR>(out, ibuf1, width, height, start_scanline, num_scanlines, xform);
            break;
    }
}

void transform_360_map(float* out, int width, int height, int start_scanline, int num_scanlines, double yaw, double pitch, double roll) {

    int w = width;
    int h = height;

    double yawR = DEG2RADF(yaw);
    double pitchR = DEG2RADF(pitch);
    double rollR = DEG2RADF(roll);

    Matrix3 xform;

    xform.identity();
    rotateX(xform, rollR);
    rotateY(xform, pitchR);
    rotateZ(xform, yawR);

    int xi, yi;
    double xt, yt;

    Vector3 ray;
    Vector3 ray2;

    for (yi = start_scanline; yi < start_scanline + num_scanlines; yi++) {
        double phi = M_PI * ((double) yi - h / 2) / h;
        double sin_phi = sin(phi);
        double cos_phi = cos(phi);
        for (xi = 0; xi < w; xi++) {
            double theta = 2 * M_PI * ((double) xi - w / 2) / w;

            ray[0] = cos(theta) * cos_phi;
            ray[1] = sin(theta) * cos_phi;
            ray[2] = sin_phi;

            mulM3V3(xform, ray, ray2);

            double theta_out = atan2 (ray2[1], ray2[0]);
            double dxy = sqrt(ray2[0] * ray2[0] + ray2[1] * ray2[1]);
            double phi_out = atan2 (ray2[2], dxy);

            xt = w / 2 + (w / 2) * theta_out / M_PI;
            yt = h / 2 + (h / 2) * phi_out / (M_PI / 2);

            if (xt < 0) {
                xt += w;
            }
            if (xt >= w) {
                xt -= w;
            }

            if (yt < 0) {
                yt = 0;
            }
            if (yt > h - 1) {
                yt = h - 1;
            }

            int idx = 2 * (yi * width + xi);
            out[idx    ] = (float) xt;
            out[idx + 1] = (float) yt;
        }
    }
}
