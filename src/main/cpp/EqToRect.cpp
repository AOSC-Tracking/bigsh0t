/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>
#include <mutex>

#include "frei0r.hpp"
#include "Math.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "ImageProcessing.hpp"
#include "Frei0rParameter.hpp"
#include "Frei0rFilter.hpp"
#include "Version.hpp"

#define SEQ_HEMI_TO_EQUIRECT_PROJECTION_EQUIDISTANT_FISHEYE 1
#define INTERP_NONE 0


class EqToRect : public Frei0rFilter, MPFilter {

  public:
    Frei0rParameter<double,double> yaw;
    Frei0rParameter<double,double> pitch;
    Frei0rParameter<double,double> roll;
    Frei0rParameter<double,double> fov;
    Frei0rParameter<double,double> fisheye;
    Frei0rParameter<int,double> interpolation;
    bool updateMap;
    float* map;

    std::mutex lock;

    EqToRect(unsigned int width, unsigned int height) : Frei0rFilter(width, height) {
        yaw = 0.0;
        pitch = 0.0;
        roll = 0.0;
        fisheye = 0.0;
        fov = 90.0;

        interpolation = Interpolation::BILINEAR;

        map = NULL;

        register_fparam(yaw, "yaw", "");
        register_fparam(pitch, "pitch", "");
        register_fparam(roll, "roll", "");
        register_fparam(fov, "fov", "");
        register_fparam(fisheye, "fisheye", "");
        register_fparam(interpolation, "interpolation", "");
    }

    ~EqToRect() {
        if (map != NULL) {
            free(map);
        }
    }

    virtual void update(double time,
                        uint32_t* out,
                        const uint32_t* in) {

        // frei0r filter instances are not thread-safe. Shotcut ignores that, so we'll
        // deal with it by wrapping the execution in a mutex
        std::lock_guard<std::mutex> guard(lock);

        if (map == NULL || yaw.changed() || pitch.changed() || roll.changed() || fov.changed() || fisheye.changed()) {
            if (map == NULL) {
                map = (float*) malloc (2 * width * height * sizeof(float));
            }
            updateMap = true;
        }

        MPFilter::updateMP(this, time, out, in, width, height);
    }

    virtual void updateLines(double time,
                             uint32_t* out,
                             const uint32_t* in, int start, int num) {
        if (updateMap) {
            make_map(start, num);
        }
        apply_360_map (out, (uint32_t*) in, map, width, height, start, num, interpolation);
    }

  protected:
    void make_map(int start_scanline, int num_scanlines) {

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

        Vector3 rray;
        rray.zero();

        Vector3 fray;
        fray.zero();

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

        double fe = fisheye / 100;
        double ife = 1.0 - fe;
        double maxdc = sqrt(width * width + height * height) / 2.0;

        bool fisheyeEnabled = true;
        bool rectilinearEnabled = true;

        if (fov > 179.9) {
            fe = 1.0;
            ife = 0.0;
        }

        if (fe > 0.99) {
            fisheyeEnabled = true;
            rectilinearEnabled = false;
        } else if (fe < 0.01) {
            fisheyeEnabled = false;
            rectilinearEnabled = true;
        }


        for (yi = start_scanline; yi < start_scanline + num_scanlines; yi++) {
            for (xi = 0; xi < w; xi++) {
                int midx = (yi * width + xi) * 2;

                if (rectilinearEnabled) {
                    rray[0] = 1.0;
                    rray[1] = topLeft[1] + xi * delta[1];
                    rray[2] = topLeft[2] + yi * delta[2];
                    mulV3S(rray, ife, rray);
                }

                if (fisheyeEnabled) {
                    double dx = xi;
                    dx -= (width / 2.0);
                    double dy = yi;
                    dy -= (height / 2.0);

                    double dc = DEG2RADF(fov / 2) * sqrt(dx * dx + dy * dy) / maxdc;
                    double ang = atan2(dy, dx);

                    if (dc > M_PI) {
                        map[midx] = -1;
                        continue;
                    }

                    fray[0] = cos(dc);
                    fray[1] = sin(dc) * cos(ang);
                    fray[2] = sin(dc) * sin(ang);
                    mulV3S(fray, fe, fray);
                }

                addV3V3(rray, fray, ray);

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

                map[midx] = (float) xt;
                map[midx + 1] = (float) yt;
            }
        }
    }


  private:

};

frei0r::construct<EqToRect> plugin("eq_to_rect",
                                   "Extracts a rectilinear image from an equirectangular.",
                                   "Leo Sutic <leo@sutic.nu>",
                                   BIGSH0T_VERSION_MAJOR, BIGSH0T_VERSION_MINOR, F0R_COLOR_MODEL_PACKED32);
