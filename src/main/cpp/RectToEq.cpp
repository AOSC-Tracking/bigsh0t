/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "ImageProcessing.hpp"
#include <limits>
#include <climits>
#include <cmath>
#include <mutex>
#include <string.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#define DEG2RADF(x) ((x) * M_PI / 180.0)

class RectToEq : public frei0r::filter, MPFilter {

public:
    double hfov;
    double vfov;
    double interpolationParam;
    int interpolation;

	std::mutex lock;

    RectToEq(unsigned int width, unsigned int height) {
        register_param(hfov, "hfov", "");
        register_param(vfov, "vfov", "");
        register_param(interpolationParam, "interpolation", "");

        hfov = 90;
        vfov = 60;

        interpolation = Interpolation::BILINEAR;
    }

    ~RectToEq() {
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
        rect_to_eq_thread(out, (uint32_t*) in, start, num);
    }

protected:
    void rect_to_eq_thread(uint32_t* out, uint32_t* ibuf1, int start_scanline, int num_scanlines) {

        int w = width;
        int h = height;

        double hfovR = DEG2RADF(hfov);
        double vfovR = DEG2RADF(vfov);

        int xi, yi;
        double xt, yt;

        double x0 = -tan(hfovR / 2);
        double y0 = -tan(vfovR / 2);
        double xs = -2 * x0;
        double ys = -2 * y0;

        Vector3 ray;

        int x_width = (int) (hfovR * w / (2 * M_PI));

        int min_x = w / 2 - x_width / 2 - 1;
        int max_x = w / 2 + x_width / 2 + 1;
        if (min_x < 0) {
            min_x = 0;
        }
        if (max_x > w - 1) {
            max_x = w - 1;
        }

        memset(&out[start_scanline * width], 0, num_scanlines * width * 4);

        for (yi = start_scanline; yi < start_scanline + num_scanlines; yi++) {
            double phi = M_PI * ((double) yi - h / 2) / h;
            double cos_phi = cos(phi);
            double sin_phi = sin(phi);
            for (xi = min_x; xi < max_x; xi++) {
                double theta = 2 * M_PI * ((double) xi - w / 2) / w;

                ray[0] = cos(theta) * cos_phi;

                if (ray[0] > 0) {
                    ray[1] = sin(theta) * cos_phi;
                    ray[2] = sin_phi;

                    ray[1] /= ray[0];
                    ray[2] /= ray[0];

                    xt = w * (ray[1] - x0) / xs;
                    yt = h * (ray[2] - y0) / ys;

                    if (xt >= 0 && yt >= 0 && xt < w - 1 && yt < h - 1) {
                        uint32_t pixel;
                        switch(interpolation) {
                            case Interpolation::NONE:
                                pixel = sampleNearestNeighbor(ibuf1, xt, yt, width, height);
                                break;
                            case Interpolation::BILINEAR:
                                pixel = sampleBilinear(ibuf1, xt, yt, width, height);
                                break;
                        }
                        out[((int) yi) * width + ((int) xi)] = pixel;
                    }
                }
            }
        }
    }


private:

};

frei0r::construct<RectToEq> plugin("rect_to_eq",
                "Converts a rectilinear image to an equirectangular map.",
                "Leo Sutic <leo@sutic.nu>",
                2, 2, F0R_COLOR_MODEL_PACKED32);
