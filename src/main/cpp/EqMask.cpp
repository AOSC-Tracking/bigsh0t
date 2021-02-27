/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>
#include <mutex>

#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "Frei0rParameter.hpp"
#include "Frei0rFilter.hpp"
#include "Math.hpp"


class EqMask : public Frei0rFilter, MPFilter {

  public:
    Frei0rParameter<double,double> hfov0;
    Frei0rParameter<double,double> hfov1;
    Frei0rParameter<double,double> vfov0;
    Frei0rParameter<double,double> vfov1;
    int interpolation;

    std::mutex lock;

    unsigned char* map;
    bool updateMap;

    EqMask(unsigned int width, unsigned int height) : Frei0rFilter(width, height) {
        hfov0 = 160.0;
        hfov1 = 180.0;
        vfov0 = 120.0;
        vfov1 = 140.0;

        map = NULL;

        interpolation = 0;

        register_fparam(hfov0, "hfov0", "");
        register_fparam(hfov1, "hfov1", "");
        register_fparam(vfov0, "vfov0", "");
        register_fparam(vfov1, "vfov1", "");
    }

    ~EqMask() {
        if (map != NULL) {
            free (map);
        }
    }

    virtual void update(double time,
                        uint32_t* out,
                        const uint32_t* in) {

        // frei0r filter instances are not thread-safe. Shotcut ignores that, so we'll
        // deal with it by wrapping the execution in a mutex
        std::lock_guard<std::mutex> guard(lock);

        if (map == NULL || hfov0.changed() || hfov1.changed() || vfov0.changed() || vfov1.changed()) {
            if (map == NULL) {
                map = (unsigned char*) malloc (width * height);
            }
            updateMap = true;
        } else {
            updateMap = false;
        }

        MPFilter::updateMP(this, time, out, in, width, height);
    }

    virtual void updateLines(double time,
                             uint32_t* out,
                             const uint32_t* in,
                             int start, int num) {
        if (updateMap) {
            makeMap(start, num);
        }
        applyMap(out, in, start, num);
    }

    void applyMap (uint32_t* out,
                   const uint32_t* in,
                   int start, int num) {

        for (int y = start; y < (start + num); ++y) {
            for (unsigned int x = 0; x < width; ++x) {
                int offset = y * width + x;
                int vInt = map[offset];
                unsigned char* inP = (unsigned char*) (in + offset);
                unsigned char* outP = (unsigned char*) (out + offset);
                for (int c = 0; c < 3; ++c) {
                    outP[c] = inP[c] * vInt >> 8;
                }
            }
        }
    }

    void makeMap (int start, int num) {
        double coshfov0 = cos(DEG2RADF(hfov0.read()) / 2);
        double coshfov1 = cos(DEG2RADF(hfov1.read()) / 2);
        double coshfovd = coshfov0 - coshfov1;

        double sinvfov0 = sin(DEG2RADF(vfov0.read()) / 2);
        double sinvfov1 = sin(DEG2RADF(vfov1.read()) / 2);
        double sinvfovd = sinvfov0 - sinvfov1;

        for (int y = start; y < (start + num); ++y) {
            double phi = (-M_PI / 2) + (y * M_PI / height);
            double cosPhi = cos(phi);
            double sinPhi = sin(phi);

            double ry = sinPhi;

            double vv = 1.0;
            if (ry > sinvfov1) {
                vv = 0.0;
            } else if (ry > sinvfov0) {
                vv = (ry - sinvfov1) / sinvfovd;
            }
            vv = smooth (vv);

            for (unsigned int x = 0; x < width; ++x) {
                double theta = M_PI - (2 * M_PI * x / width);
                double cosTheta = cos(theta);

                double z = cosTheta * cosPhi;

                double vh = 1.0;
                if (z < coshfov1) {
                    vh = 0.0;
                } else if (z < coshfov0) {
                    vh = (z - coshfov1) / coshfovd;
                }

                vh = smooth (vh);

                double v = vh * vv;
                int vInt = (int) (v * 256);
                if (vInt > 255) {
                    vInt = 255;
                }
                if (vInt < 0) {
                    vInt = 0;
                }
                int offset = y * width + x;
                map[offset] = (unsigned char) vInt;
            }
        }
    }

  protected:
    double smooth (double v) {
        double v2 = 1.0 - (1.0 - v) * (1.0 - v);
        double blend = 1.0 - v;
        return v * blend + (1.0 - blend) * v2;
    }

  private:

};

frei0r::construct<EqMask> plugin("eq_mask",
                                 "Masks part of the VR sphere.",
                                 "Leo Sutic <leo@sutic.nu>",
                                 2, 2, F0R_COLOR_MODEL_RGBA8888);
