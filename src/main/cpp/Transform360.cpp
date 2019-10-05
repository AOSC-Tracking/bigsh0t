/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>
#include <mutex>
#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "ImageProcessing.hpp"
#include "Frei0rParameter.hpp"
#include "Frei0rFilter.hpp"

#define SEQ_HEMI_TO_EQUIRECT_PROJECTION_EQUIDISTANT_FISHEYE 1
#define INTERP_NONE 0


class Transform360 : public Frei0rFilter, MPFilter {

public:
    Frei0rParameter<double,double> yaw;
	Frei0rParameter<double,double> pitch;
    Frei0rParameter<double,double> roll;
    Frei0rParameter<int,double> interpolation;
	bool updateMap;
	float* map;

	int mapHits;

	std::mutex lock;

    Transform360(unsigned int width, unsigned int height) : Frei0rFilter(width, height) {
        yaw = 0.0;
        pitch = 0.0;
        roll = 0.0;

		map = NULL;

		mapHits = 24;

        interpolation = Interpolation::BILINEAR;

        register_fparam(yaw, "yaw", "");
        register_fparam(pitch, "pitch", "");
        register_fparam(roll, "roll", "");
        register_fparam(interpolation, "interpolation", "");
    }

    ~Transform360() {
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

		if (map == NULL || yaw.changed() || pitch.changed() || roll.changed()) {
			if (map == NULL) {
				map = (float*) malloc (2 * width * height * sizeof(float));
			}
			updateMap = true;
			--mapHits;
			if (mapHits < 0) {
				mapHits = 0;
			}
		} else {
			++mapHits;
			if (mapHits > 32) {
				mapHits = 32;
			}
			updateMap = false;
			if (mapHits == 17) {
				updateMap = true;
			}
		}
        MPFilter::updateMP(this, time, out, in, width, height);
    }

	virtual void updateLines(double time,
	                    uint32_t* out,
                        const uint32_t* in, int start, int num) {
		if (mapHits > 16) {
			if (updateMap) {
				transform_360_map(map, width, height, start, num, yaw, pitch, roll);
			}
			apply_360_map (out, (uint32_t*) in, map, width, height, start, num, interpolation);
		} else {
			transform_360(out, (uint32_t*) in, width, height, start, num, yaw, pitch, roll, interpolation);
		}
    }
};

frei0r::construct<Transform360> plugin("transform_360",
                "Rotates an equirectangular map.",
                "Leo Sutic <leo@sutic.nu>",
                2, 1, F0R_COLOR_MODEL_PACKED32);
