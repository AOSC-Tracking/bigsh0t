/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>
#include <algorithm>
#include <iomanip>
#ifdef USE_SSE
#include <emmintrin.h>
#include <mmintrin.h>
#include <intrin.h>
#endif

#include "ImageProcessing.hpp"
#include "Matrix.hpp"
#include "Math.hpp"

uint32_t sampleNearestNeighbor (const uint32_t* frame, double x, double y, int width, int height) {
    return frame[((int) y) * width + ((int) x)];
}

#define EXPAND_ABGR64(v) ( (v & 0xff000000) << 24 ) | ( (v & 0x00ff0000) << 16 ) | ( (v & 0x0000ff00) << 8 ) | ( (v & 0x000000ff) )
#define COMPRESS_ABGR64(v) ( (v >> 24) & 0xff000000 ) | ( (v >> 16) & 0x00ff0000) | ( (v >> 8) & 0x0000ff00 ) | ( v & 0x000000ff )


#ifdef USE_SSE
void dumpSSE(char* desc, __m128i reg) {
    std::cout << desc << ": ";
    for (int i = 0; i < 2; ++i) {
        uint64_t result = _mm_cvtsi128_si64(reg);
        std::cout << std::setw(16) << std::setfill('0') << std::hex << result << std::dec;
        reg = _mm_bsrli_si128(reg, 8);
    }
    std::cout << std::endl;
}

inline uint32_t _sseBlerp(const uint32_t* frame, int ai, int bi, int ci, int di, int ax, int ay, int width, int height) {
    uint32_t a = frame[ai];
    uint32_t b = frame[bi];
    uint32_t c = frame[ci];
    uint32_t d = frame[di];

    __m128i A = _mm_cvtsi32_si128(a);
    A = _mm_unpacklo_epi8(A, _mm_setzero_si128());
    //dumpSSE("A", A);

    __m128i B = _mm_cvtsi32_si128(b);
    B = _mm_unpacklo_epi8(B, _mm_setzero_si128());
    //dumpSSE("B", B);

    __m128i C = _mm_cvtsi32_si128(c);
    C = _mm_unpacklo_epi8(C, _mm_setzero_si128());
    //dumpSSE("C", C);

    __m128i D = _mm_cvtsi32_si128(d);
    D = _mm_unpacklo_epi8(D, _mm_setzero_si128());
    //dumpSSE("D", D);

    __m128i AC = _mm_add_epi16(A, _mm_bslli_si128(C, 8));
    //dumpSSE("AC", AC);
    __m128i BD = _mm_add_epi16(B, _mm_bslli_si128(D, 8));
    //dumpSSE("BD", BD);

    __m128i BD_minus_AC = _mm_sub_epi16(BD, AC);
    //dumpSSE(" -", BD_minus_AC);
    __m128i AX = _mm_set1_epi16(ax);
    __m128i X_ADDN_shifted = _mm_mullo_epi16(BD_minus_AC, AX);
    __m128i X_ADDN = _mm_srai_epi16(X_ADDN_shifted, 7);
    //dumpSSE("X_ADDN", X_ADDN);

    __m128i EF = _mm_add_epi16(AC, X_ADDN);
    //dumpSSE("EF", EF);
    // EF now has E1E2E3E4F1F2F3F4

    // __m128i E = (A + (((B - A) * ax) >> 8) & 0x00ff00ff00ff00ff);
    // __m128i F = (C + (((D - C) * ax) >> 8) & 0x00ff00ff00ff00ff);

    // uint64_t G = E + (((F - E) * ay) >> 8);

    __m128i E = EF;
    __m128i F = _mm_bsrli_si128(EF, 8);
    //dumpSSE("E", E);
    //dumpSSE("F", F);

    __m128i F_minus_E = _mm_sub_epi16(F, E);
    //dumpSSE("F_minus_E", F_minus_E);
    __m128i AY = _mm_set1_epi16(ay);
    //dumpSSE("AY", AY);
    __m128i Y_ADDN_shifted = _mm_mullo_epi16(F_minus_E, AY);
    //dumpSSE("Y_ADDN_shifted", Y_ADDN_shifted);
    __m128i Y_ADDN = _mm_srai_epi16(Y_ADDN_shifted, 7);
    //dumpSSE("Y_ADDN", Y_ADDN);

    __m128i G = _mm_add_epi16(E, Y_ADDN);

    __m128i repacked = _mm_packus_epi16(G, G);
    uint32_t result = _mm_cvtsi128_si32(repacked);

    return result;
}

uint32_t sseBlerp(const uint32_t* frame, int ai, int bi, int ci, int di, int ax, int ay, int width, int height) {
    return _sseBlerp(frame, ai, bi, ci, di, ax, ay, width, height);
}
#endif

inline uint32_t _int64Blerp(const uint32_t* frame, int ai, int bi, int ci, int di, int ax, int ay, int width, int height) {
    uint64_t a = frame[ai];
    uint64_t b = frame[bi];
    uint64_t c = frame[ci];
    uint64_t d = frame[di];

    uint64_t A = EXPAND_ABGR64(a);
    uint64_t B = EXPAND_ABGR64(b);
    uint64_t C = EXPAND_ABGR64(c);
    uint64_t D = EXPAND_ABGR64(d);

    uint64_t E = (A + (((B - A) * ax) >> 7) & 0x00ff00ff00ff00ff);
    uint64_t F = (C + (((D - C) * ax) >> 7) & 0x00ff00ff00ff00ff);

    uint64_t G = E + (((F - E) * ay) >> 7);
    return COMPRESS_ABGR64(G);
}

uint32_t int64Blerp(const uint32_t* frame, int ai, int bi, int ci, int di, int ax, int ay, int width, int height) {
    return _int64Blerp(frame, ai, bi, ci, di, ax, ay, width, height);
}

inline uint32_t blerp(const uint32_t* frame, int ai, int bi, int ci, int di, int ax, int ay, int width, int height) {
#ifdef USE_SSE
    return _sseBlerp(frame, ai, bi, ci, di, ax, ay, width, height);
#else
    return _int64Blerp(frame, ai, bi, ci, di, ax, ay, width, height);
#endif
}

uint32_t sampleBilinear (const uint32_t* frame, double x, double y, int width, int height) {
    int ix0 = (int) x;
    int iy0 = (int) y;
    int ix1 = ix0 + 1;
    int iy1 = iy0 + 1;
    int ax = (int) ((x - ix0) * 128);
    int ay = (int) ((y - iy0) * 128);

    if (ix0 < 0 || iy0 < 0 || ix0 > width - 1 || iy0 > height - 1) {
        return 0;
    }
    if (ix1 >= width) {
        ix1 = width - 1;
    }
    if (iy1 >= height) {
        iy1 = height - 1;
    }


    int iy0w = iy0 * width;
    int iy1w = iy1 * width;

    return blerp(frame, iy0w + ix0, iy0w + ix1, iy1w + ix0, iy1w + ix1, ax, ay, width, height);
}

#define CLAMP_ANY(val,lo,hi) ( val < lo ) ? lo : (hi < val ? hi : val)
#define WRAP_ANY(val,hi) ( val >= hi ) ? (val - hi) : (val < 0 ? (val + hi) : val)


uint32_t sampleBilinearWrappedClamped (const uint32_t* frame, double x, double y, int width, int height) {
    int ix0 = (int) x;
    int iy0 = (int) y;
    int ix1 = ix0 + 1;
    int iy1 = iy0 + 1;
    int ax = (int) ((x - ix0) * 128);
    int ay = (int) ((y - iy0) * 128);

    ix0 = WRAP_ANY(ix0, width);
    ix1 = WRAP_ANY(ix1, width);
    int hm1 = height - 1;
    iy0 = CLAMP_ANY(iy0, 0, hm1);
    iy1 = CLAMP_ANY(iy1, 0, hm1);

    int iy0w = iy0 * width;
    int iy1w = iy1 * width;

    return blerp(frame, iy0w + ix0, iy0w + ix1, iy1w + ix0, iy1w + ix1, ax, ay, width, height);
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
