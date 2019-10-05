/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "ImageProcessing.hpp"
#include "Frei0rParameter.hpp"
#include "Frei0rFilter.hpp"
#include <limits>
#include <climits>
#include <cmath>
#include <mutex>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
#define DEG2RADF(x) ((x) * M_PI / 180.0)

enum Projection {
    EQUIDISTANT_FISHEYE = 0
};

class HemiToEquirect : public Frei0rFilter, MPFilter {

public:
    Frei0rParameter<double,double> yaw;
	Frei0rParameter<double,double> pitch;
    Frei0rParameter<double,double> roll;
    Frei0rParameter<int,double> interpolation;
    Frei0rParameter<int,double> projection;
    Frei0rParameter<double,double> fov;
    Frei0rParameter<double,double> radius;
    Frei0rParameter<double,double> frontX;
    Frei0rParameter<double,double> frontY;
    Frei0rParameter<double,double> frontUp;
    Frei0rParameter<double,double> backX;
    Frei0rParameter<double,double> backY;
    Frei0rParameter<double,double> backUp;
    Frei0rParameter<double,double> nadirRadius;
    Frei0rParameter<double,double> nadirCorrectionStart;

	std::mutex lock;

	float* map;
	bool updateMap;

    HemiToEquirect(unsigned int width, unsigned int height) : Frei0rFilter (width, height) {
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

		map = NULL;
		updateMap = true;

        register_fparam(yaw, "yaw", "");
        register_fparam(pitch, "pitch", "");
        register_fparam(roll, "roll", "");

        register_fparam(fov, "fov", "");
        register_fparam(radius, "radius", "");

        register_fparam(nadirRadius, "nadirRadius", "");
        register_fparam(nadirCorrectionStart, "nadirCorrectionStart", "");

        register_fparam(frontX, "frontX", "");
        register_fparam(frontY, "frontY", "");
        register_fparam(frontUp, "frontUp", "");

        register_fparam(backX, "backX", "");
        register_fparam(backY, "backY", "");
        register_fparam(backUp, "backUp", "");

        register_fparam(interpolation, "interpolation", "");
        register_fparam(projection, "projection", "");
    }

    ~HemiToEquirect() {
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

        if (map == NULL || yaw.changed() || pitch.changed() || roll.changed() || projection.changed() || fov.changed() ||
    		radius.changed() || frontX.changed() || frontY.changed() || frontUp.changed() || backX.changed() || backY.changed() ||
			backUp.changed() || nadirRadius.changed() || nadirCorrectionStart.changed()) {
			if (map == NULL) {
				map = (float*) malloc (width * height * 5 * sizeof(float));
			}
			updateMap = true;
		} else {
			updateMap = false;
		}

        MPFilter::updateMP(this, time, out, in, width, height);
    }

    virtual void updateLines(double time,
	                    uint32_t* out,
                        const uint32_t* in, int start, int num) {
		if (updateMap) {
        	makeMap (start, num);
		}

		applyMap(out, (uint32_t*) in, start, num);
    }

protected:
    void sample (int mx, int my, int mz, double fov2, double radius, double thetaH, double phi, double up_dir, Matrix3& hemi_transform,
                 double nadir_correction_start, double nadir_radius_scale, double cx, double cy) {
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

		int midx = ((my * width) + mx) * 5 + 2 * mz;

        if (srcX >= 0 && srcY >= 0 && srcX < width && srcY < height) {
			map[midx + 0] = (float) srcX;
			map[midx + 1] = (float) srcY;
        } else {
            map[midx + 0] = -1;
			map[midx + 1] = -1;
        }
    }

	uint32_t sampleImage (const uint32_t* in, float x, float y) {
		switch(interpolation) {
            case Interpolation::NONE:
                return sampleNearestNeighbor(in, x, y, width, height);
            case Interpolation::BILINEAR:
                return sampleBilinear(in, x, y, width, height);
        }
		return 0;
	}

	void applyMap(uint32_t* out, const uint32_t* in, int start_scanline, int num_scanlines) {
		for (int yi = start_scanline; yi < start_scanline + num_scanlines; yi++) {
            for (int xi = 0; xi < width; xi++) {
				int idx = ((yi * width) + xi);
				int midx = idx * 5;
				if (map[midx] > 0) {
					if (map[midx + 2] < 0) {
						// No blend

						float sx = map[midx + 0];
						float sy = map[midx + 1];
						out[idx] = sampleImage (in, sx, sy);
					} else {
						// Blend
						float sxA = map[midx + 0];
						float syA = map[midx + 1];
						uint32_t blendA = sampleImage (in, sxA, syA);

						float sxB = map[midx + 2];
						float syB = map[midx + 3];
						uint32_t blendB = sampleImage (in, sxB, syB);

						float blend = map[midx + 4];

						unsigned char* blendCA = (unsigned char*) &blendA;
						unsigned char* blendCB = (unsigned char*) &blendB;
						unsigned char* blendOut = (unsigned char*) (out + idx);

						for (int c = 0; c < 4; ++c) {
	                        blendOut[c] = (unsigned char) (blendCB[c] * blend + (1 - blend) * blendCA[c]);
	                    }
					}
				}
			}
		}
	}

    void makeMap (int start_scanline, int num_scanlines) {

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

        for (int yi = start_scanline; yi < start_scanline + num_scanlines; yi++) {
            double phi = M_PI * ((double) yi - h / 2) / h;
            for (int xi = 0; xi < w; xi++) {
                double theta = 2 * M_PI * ((double) xi - w / 2) / w;

				int midx = ((yi * width) + xi) * 5;

                if (theta < -M_PI / 2 - theta_margin || theta > M_PI / 2 + theta_margin) {
                    if (theta < 0) {
                        theta = theta + M_PI;
                    } else {
                        theta = theta - M_PI;
                    }

                    sample(xi, yi, 0, fov2, pixelRadius, theta, phi, back_up, xform_back, nadirCorrectionStart, nadir_radius_scale, back_x, back_y);
					map[midx + 2] = -1;
                } else if (theta > -M_PI / 2 + theta_margin && theta < M_PI / 2 - theta_margin) {
                    sample(xi, yi, 0, fov2, pixelRadius, theta, phi, front_up, xform_front, nadirCorrectionStart, nadir_radius_scale, front_x, front_y);
					map[midx + 2] = -1;
                } else {
                    sample(xi, yi, 0, fov2, pixelRadius, theta, phi, front_up, xform_front, nadirCorrectionStart, nadir_radius_scale, front_x, front_y);
					float blend = 0.0f;
                    if (theta < 0) {
                        blend = -(float) ((theta + M_PI / 2 - theta_margin) / (2 * theta_margin));
                        theta = theta + M_PI;
                    } else {
                        blend = (float) ((theta - M_PI / 2 + theta_margin) / (2 * theta_margin));
                        theta = theta - M_PI;
                    }
                    sample(xi, yi, 1, fov2, pixelRadius, theta, phi, back_up, xform_back, nadirCorrectionStart, nadir_radius_scale, back_x, back_y);

					map[midx + 4] = blend;
                }
            }
        }
    }


private:

};

frei0r::construct<HemiToEquirect> plugin("hemi_to_eq",
                "Transforms dual-hemisphere video to equirectangular.",
                "Leo Sutic <leo@sutic.nu>",
                2, 1, F0R_COLOR_MODEL_PACKED32);
