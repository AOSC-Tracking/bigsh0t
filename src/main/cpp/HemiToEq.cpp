/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "ImageProcessing.hpp"
#include <limits>
#include <climits>
#include <cmath>

#define M_PI           3.14159265358979323846  
#define DEG2RADF(x) ((x) * M_PI / 180.0)

enum Projection {
    EQUIDISTANT_FISHEYE = 0
};


class HemiToEquirect : public frei0r::filter, MPFilter {

public:
    double yaw;
	double pitch;
    double roll;
    int interpolation;
    double interpolationParam;
    int projection;
    double projectionParam;
    double fov;
    double radius;
    double frontX;
    double frontY;
    double frontUp;
    double backX;
    double backY;
    double backUp;
    double nadirRadius;
    double nadirCorrectionStart;
	
    
    HemiToEquirect(unsigned int width, unsigned int height) {
        register_param(yaw, "yaw", "");
        register_param(pitch, "pitch", "");
        register_param(roll, "roll", "");
        
        register_param(fov, "fov", "");
        register_param(radius, "radius", "");
        
        register_param(nadirRadius, "nadirRadius", "");
        register_param(nadirCorrectionStart, "nadirCorrectionStart", "");
        
        register_param(frontX, "frontX", "");
        register_param(frontY, "frontY", "");
        register_param(frontUp, "frontUp", "");
        
        register_param(backX, "backX", "");
        register_param(backY, "backY", "");
        register_param(backUp, "backUp", "");
        
        register_param(interpolationParam, "interpolation", "");
        register_param(projectionParam, "projection", "");
        
        yaw = 0.357f;
        pitch = 0.389f;
        roll = -0.693f;

        interpolation = Interpolation::NONE;
        projection = Projection::EQUIDISTANT_FISHEYE;

        fov = 182.8697f;
        radius = 430.0 / 1920.0f;
        frontX = 0.75;
        frontY = 480.f / 1080.0f;
        frontUp = 90.0f;
        backX = 0.25;
        backY = 480.f / 1080.0f;
        backUp = 270.0f;
        nadirRadius = 428.0 / 1920.0f;
        nadirCorrectionStart = 0.8f;
    }

    ~HemiToEquirect() {
    }
    
    virtual void update(double time,
	                    uint32_t* out,
                        const uint32_t* in) {
        interpolation = (int) interpolationParam;
        projection = (int) projectionParam;
        MPFilter::updateMP(this, time, out, in, width, height);
    }

    virtual void updateLines(double time,
	                    uint32_t* out,
                        const uint32_t* in, int start, int num) {
        hemi_to_equirect_thread(out, (uint32_t*) in, start, num);
    }

protected:
    void sample (uint32_t *ibuf1, double fov2, double radius, double thetaH, double phi, double up_dir, int hemisphere, Matrix3& hemi_transform,
                 double nadir_correction_start, double nadir_radius_scale, uint32_t* out, double cx, double cy) {
        Vector3 ray;
        Vector3 ray2;

        double cos_phi = cos(phi);
        ray[0] = cos(-thetaH) * cos_phi;
        ray[1] = sin(-thetaH) * cos_phi;
        ray[2] = sin(phi);

        mulM3V3 (hemi_transform, ray, ray2);

        double off_axis = sqrt(ray2[1] * ray2[1] + ray2[2] * ray2[2]);
        double off_axis_direction = atan2(-ray2[2], ray2[1]);

        double off_axis_down = -cos(up_dir - off_axis_direction) * off_axis;

        double off_axis_angle = atan2(off_axis, ray2[0]);

        if (off_axis_down > nadir_correction_start) {
            double factor = 1.0 - (1.0 - nadir_radius_scale) * (off_axis_down - nadir_correction_start) / (1.0 - nadir_correction_start);
            off_axis_angle *= factor;
        }

        double srcX, srcY;

        switch (projection) {
            case Projection::EQUIDISTANT_FISHEYE:
                srcX = ((cos(off_axis_direction) * off_axis_angle / fov2) * radius);
                srcY = ((sin(off_axis_direction) * off_axis_angle / fov2) * radius);
                break;
        }

        srcX += cx;
        srcY += cy;

        if (srcX >= 0 && srcY >= 0 && srcX < width && srcY < height) {
            uint32_t pixel;
            switch(interpolation) {
                case Interpolation::NONE:
                    pixel = sampleNearestNeighbor(ibuf1, srcX, srcY, width, height);
                    break;
                case Interpolation::BILINEAR:
                    pixel = sampleBilinear(ibuf1, srcX, srcY, width, height);
                    break;
            }
            *out = pixel;
        } else {
            *out = 0;
        }
    }

    void hemi_to_equirect_thread(uint32_t* out, uint32_t* ibuf1, int start_scanline, int num_scanlines) {
        
        int w = width;
        int h = height;

        double yawR = DEG2RADF(yaw);
        double pitchR = DEG2RADF(pitch);
        double rollR = DEG2RADF(roll);

        Matrix3 xform_front;
        Matrix3 xform_back;

        xform_front.identity();
        rotateX(xform_front, DEG2RADF(180.0 - frontUp));
        rotateZ(xform_front, yawR / 2);
        rotateY(xform_front, pitchR / 2);
        rotateX(xform_front, rollR / 2);

        xform_back.identity();
        rotateX(xform_back, DEG2RADF(180.0 - backUp));
        rotateZ(xform_back, -yawR / 2);
        rotateY(xform_back, pitchR / 2);
        rotateX(xform_back, rollR / 2);

        int xi, yi;

        double fov90radius = 90.0f * (radius * 2) / fov;
        double fov2 = (radius * 90.0f / fov90radius) * 2 * M_PI / 360.0f;

        double theta_margin = fov2 - M_PI / 2;
        double nadir_radius_scale = nadirRadius / radius;

        double pixelRadius = radius * w;
        
        double front_x = frontX * w;
        double front_y = frontY * h;
        double front_up = DEG2RADF(90 - frontUp);

        double back_x = backX * w;
        double back_y = backY * h;
        double back_up = DEG2RADF(90 - backUp);
        
        unsigned char blendBuffer[4];

        for (yi = start_scanline; yi < start_scanline + num_scanlines; yi++) {
            double phi = M_PI * ((double) yi - h / 2) / h;
            for (xi = 0; xi < w; xi++) {
                double theta = 2 * M_PI * ((double) xi - w / 2) / w;

                size_t offset = w * yi + xi;

                if (theta < -M_PI / 2 - theta_margin || theta > M_PI / 2 + theta_margin) {
                    if (theta < 0) {
                        theta = theta + M_PI;
                    } else {
                        theta = theta - M_PI;
                    }

                    sample(ibuf1, fov2, pixelRadius, theta, phi, back_up, 0, xform_back, nadirCorrectionStart, nadir_radius_scale, out + offset, back_x, back_y);
                } else if (theta > -M_PI / 2 + theta_margin && theta < M_PI / 2 - theta_margin) {
                    sample(ibuf1, fov2, pixelRadius, theta, phi, front_up, 1, xform_front, nadirCorrectionStart, nadir_radius_scale, out + offset, front_x, front_y);
                } else {
                    sample(ibuf1, fov2, pixelRadius, theta, phi, front_up, 1, xform_front, nadirCorrectionStart, nadir_radius_scale, (uint32_t*) blendBuffer, front_x, front_y);
                    double blend = 0.0f;
                    if (theta < 0) {
                        blend = -(double) ((theta + M_PI / 2 - theta_margin) / (2 * theta_margin));
                        theta = theta + M_PI;
                    } else {
                        blend = (double) ((theta - M_PI / 2 + theta_margin) / (2 * theta_margin));
                        theta = theta - M_PI;
                    }
                    
                    unsigned char* blendBuffer2 = (unsigned char*) (out + offset);

                    sample(ibuf1, fov2, pixelRadius, theta, phi, back_up, 0, xform_back, nadirCorrectionStart, nadir_radius_scale, out + offset, back_x, back_y);

                    for (int c = 0; c < 4; ++c) {
                        blendBuffer2[c] = (unsigned char) (blendBuffer2[c] * blend + (1 - blend) * blendBuffer[c]);
                    }                    
                }
            }
        }
    }

    
private:
    
};

frei0r::construct<HemiToEquirect> plugin("hemi_to_eq",
                "Transforms dual-hemisphere video to equirectangular.",
                "Leo Sutic <leo@sutic.nu>",
                1, 0, F0R_COLOR_MODEL_PACKED32);