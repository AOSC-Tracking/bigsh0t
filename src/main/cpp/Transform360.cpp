/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "ImageProcessing.hpp"
#include <limits>
#include <climits>
#include <cmath>

#define M_PI           3.14159265358979323846  
#define SEQ_HEMI_TO_EQUIRECT_PROJECTION_EQUIDISTANT_FISHEYE 1
#define INTERP_NONE 0
#define DEG2RADF(x) ((x) * M_PI / 180.0)


class Transform360 : public frei0r::filter, MPFilter {

public:
    double yaw;
	double pitch;
    double roll;
    double interpolationParam;
    int interpolation;
	
    
    Transform360(unsigned int width, unsigned int height) {
        register_param(yaw, "yaw", "");
        register_param(pitch, "pitch", "");
        register_param(roll, "roll", "");
        register_param(interpolationParam, "interpolation", "");
        
        yaw = 0.0;
        pitch = 0.0;
        roll = 0.0;

        interpolation = Interpolation::BILINEAR;
    }

    ~Transform360() {
    }
    
    virtual void update(double time,
	                    uint32_t* out,
                        const uint32_t* in) {
        interpolation = (int) interpolationParam;
        MPFilter::updateMP(this, time, out, in, width, height);
    }
    
    virtual void updateLines(double time,
	                    uint32_t* out,
                        const uint32_t* in, int start, int num) {
        transform_360_thread(out, (uint32_t*) in, start, num);     
    }

protected:
    void transform_360_thread(uint32_t* out, uint32_t* ibuf1, int start_scanline, int num_scanlines) {
        
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

frei0r::construct<Transform360> plugin("transform_360",
                "Rotates an equirectangular map.",
                "Leo Sutic <leo@sutic.nu>",
                1, 0, F0R_COLOR_MODEL_PACKED32);