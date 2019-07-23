/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "ImageProcessing.hpp"
#include <limits>
#include <climits>
#include <cmath>
#include <mutex>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
#define SEQ_HEMI_TO_EQUIRECT_PROJECTION_EQUIDISTANT_FISHEYE 1
#define INTERP_NONE 0
#define DEG2RADF(x) ((x) * M_PI / 180.0)


class EqToRect : public frei0r::filter, MPFilter {

public:
    double yaw;
	double pitch;
    double roll;
    double fov;
    double interpolationParam;
    int interpolation;

	std::mutex lock;

    EqToRect(unsigned int width, unsigned int height) {
        register_param(yaw, "yaw", "");
        register_param(pitch, "pitch", "");
        register_param(roll, "roll", "");
        register_param(fov, "fov", "");
        register_param(interpolationParam, "interpolation", "");

        yaw = 0.0;
        pitch = 0.0;
        roll = 0.0;
        fov = 90.0;

        interpolation = Interpolation::BILINEAR;
    }

    ~EqToRect() {
    }

    virtual void update(double time,
	                    uint32_t* out,
                        const uint32_t* in) {

		// frei0r filter instances are not thread-safe. Shotcut ignores that, so we'll
		// deal with it by wrapping the execution in a mutex
		std::lock_guard<std::mutex> guard(lock);

        interpolation = (int) interpolationParam;
        MPFilter::updateMP(this, time, out, in, width, height);
    }

    virtual void updateLines(double time,
	                    uint32_t* out,
                        const uint32_t* in, int start, int num) {
        transform_thread(out, (uint32_t*) in, start, num);
    }

protected:
    void transform_thread(uint32_t* out, uint32_t* ibuf1, int start_scanline, int num_scanlines) {

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

        Vector3 topLeft;
        topLeft[0] = 1.0;
        topLeft[1] = -tan(DEG2RADF(fov / 2));
        topLeft[2] = topLeft[1] * height / width;

        Vector3 delta;
        delta[0] = 0.0;
        delta[1] = -topLeft[1] / (width / 2);
        delta[2] = -topLeft[2] / (height / 2);

        for (yi = start_scanline; yi < start_scanline + num_scanlines; yi++) {
            for (xi = 0; xi < w; xi++) {
                ray[0] = 1.0;
                ray[1] = topLeft[1] + xi * delta[1];
                ray[2] = topLeft[2] + yi * delta[2];

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


private:

};

frei0r::construct<EqToRect> plugin("eq_to_rect",
                "Extracts a rectilinear image from an equirectangular.",
                "Leo Sutic <leo@sutic.nu>",
                2, 0, F0R_COLOR_MODEL_PACKED32);
