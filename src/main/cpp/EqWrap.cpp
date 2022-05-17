/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>
#include <mutex>
#include <cstring>

#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "Frei0rParameter.hpp"
#include "Frei0rFilter.hpp"
#include "Math.hpp"
#include "ImageProcessing.hpp"
#include "SummedAreaTable.hpp"
#include "Version.hpp"


class EqWrap : public Frei0rFilter, MPFilter {

  public:
    SummedAreaTable sat;
    Frei0rParameter<int,double> interpolation;
    Frei0rParameter<double,double> hfov0;
    Frei0rParameter<double,double> hfov1;
    Frei0rParameter<double,double> vfov0;
    Frei0rParameter<double,double> vfov1;
    Frei0rParameter<double,double> blurStart;
    Frei0rParameter<double,double> blurEnd;
    Transform360Support t360;

    std::mutex lock;

    int hfov0px;
    int hfov1px;
    int vfov0px;
    int vfov1px;
    int hfovPx;
    int vfovPx;
    int blurStartPx;
    int blurEndPx;
    double pitchPixels;
    double yawPixels;

    EqWrap(unsigned int width, unsigned int height) : Frei0rFilter(width, height), sat(width, height), t360(width, height) {
        interpolation = 0;

        hfov0 = -90.0;
        hfov1 = 90.0;
        vfov0 = -45.0;
        vfov1 = 45.0;
        blurStart = 0.1;
        blurEnd = 1.0;

        register_fparam(hfov0, "hfov0", "");
        register_fparam(hfov1, "hfov1", "");
        register_fparam(vfov0, "vfov0", "");
        register_fparam(vfov1, "vfov1", "");
        register_fparam(blurStart, "blurStart", "");
        register_fparam(blurEnd, "blurEnd", "");

        register_fparam(interpolation, "interpolation", "");

        pitchPixels = height / 180.0;
        yawPixels = width / 360.0;
    }

    ~EqWrap() {
    }

    virtual void update(double time,
                        uint32_t* out,
                        const uint32_t* in) {

        // frei0r filter instances are not thread-safe. Shotcut ignores that, so we'll
        // deal with it by wrapping the execution in a mutex
        std::lock_guard<std::mutex> guard(lock);

        hfov0px = hfov0 * yawPixels + width / 2;
        hfov1px = hfov1 * yawPixels + width / 2;
        if (hfov1px <= hfov0px) {
            hfov1px = hfov0px + 1;
        }

        vfov0px = vfov0 * pitchPixels + height / 2;
        vfov1px = vfov1 * pitchPixels + height / 2;
        if (vfov1px <= vfov0px) {
            vfov1px = vfov0px + 1;
        }

        hfovPx = hfov1px - hfov0px;
        vfovPx = vfov1px - vfov0px;

        blurStartPx = blurStart * hfovPx;
        if (blurStartPx < 1) {
            blurStartPx = 1;
        }

        blurEndPx = blurEnd * hfovPx;
        if (blurEndPx < blurStartPx) {
            blurEndPx = blurStartPx;
        }

        sat.compute(in, width, hfov0px, vfov0px, hfovPx, vfovPx);

        MPFilter::updateMP(this, time, out, in, width, height);
    }

    virtual void updateLines(double time,
                             uint32_t* out,
                             const uint32_t* in,
                             int start, int num) {
        const int height2 = height / 2;
        const int width2 = width / 2;
        for (int y = start; y < start + num; ++y) {
            int distance = 0;
            int dmax = 0;
            if (y < vfov0px) {
                distance = vfov0px - y;
                dmax = vfov0px;
            } else if (y < vfov1px) {
                distance = 0;
                dmax = 1;
            } else {
                distance = y - vfov1px;
                dmax = height - vfov1px;
            }

            int sampleWidth = blurStartPx + (blurEndPx - blurStartPx) * distance / dmax;
            if (sampleWidth < 1) {
                sampleWidth = 1;
            }
            if (sampleWidth > hfovPx) {
                sampleWidth = hfovPx;
            }
            const int sampleY = vfovPx * y / height;
            uint32_t* p = &out[y * width];
            const int sampleWidth2 = sampleWidth >> 1;

            if (y < vfov0px) {
                for (int x = 0; x < width; ++x) {
                    int sampleX = hfovPx * x / width - sampleWidth2;
                    *p++ = sat.averagePixel(sampleX, sampleY, sampleWidth, 1);
                }
            } else if (y < vfov1px) {
                for (int x = 0; x < hfov0px; ++x) {
                    int sampleX = hfovPx * x / width - sampleWidth2;
                    *p++ = sat.averagePixel(sampleX, sampleY, sampleWidth, 1);
                }

                p = &out[y * width + hfov0px];
                const uint32_t* s = &in[y * width + hfov0px];
                memcpy(p, s, hfovPx * sizeof(uint32_t));

                p = &out[y * width + hfov1px];
                for (int x = hfov1px; x < width; ++x) {
                    int sampleX = hfovPx * x / width - sampleWidth2;
                    *p++ = sat.averagePixel(sampleX, sampleY, sampleWidth, 1);
                }
            } else {
                for (int x = 0; x < width; ++x) {
                    int sampleX = hfovPx * x / width - sampleWidth2;
                    *p++ = sat.averagePixel(sampleX, sampleY, sampleWidth, 1);
                }
            }
        }
    }

  protected:

  private:

};

frei0r::construct<EqWrap> plugin("bigsh0t_eq_wrap",
                                 "Stretches a partial pano sphere to cover the entire sphere.",
                                 "Leo Sutic <leo@sutic.nu>",
                                 BIGSH0T_VERSION_MAJOR, BIGSH0T_VERSION_MINOR, F0R_COLOR_MODEL_RGBA8888);
