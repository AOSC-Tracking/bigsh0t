/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef ImageProcessing_HPP
#define ImageProcessing_HPP

#include <inttypes.h>
#include "LUT.hpp"
#include "Matrix.hpp"

enum Interpolation {
    NONE = 0,
    BILINEAR = 1
};

class Transform360Support {
  public:
    Transform360Support(int width, int height);
    ~Transform360Support();

    double* cos_theta;
    double* sin_theta;
    double* cos_phi;
    double* sin_phi;
};

inline uint32_t sampleNearestNeighbor (const uint32_t* frame, double x, double y, int width, int height) {
    return frame[((int) y) * width + ((int) x)];
}

uint32_t sampleBilinear (const uint32_t* frame, double x, double y, int width, int height);
uint32_t sampleBilinearWrappedClamped (const uint32_t* frame, double x, double y, int width, int height);
#ifdef USE_SSE
uint32_t sseBlerp(const uint32_t* frame, int ai, int bi, int ci, int di, int ax, int ay, int width, int height);
#endif
uint32_t int64Blerp(const uint32_t* frame, int ai, int bi, int ci, int di, int ax, int ay, int width, int height);

uint32_t int64lerp(const uint32_t a, const uint32_t b, const int x);

uint32_t int32Scale(const uint32_t v, const uint32_t rs, const uint32_t gs, const uint32_t bs, const uint32_t shift);
uint32_t int32Scale(const uint32_t v, const uint32_t rs, const uint32_t gs, const uint32_t bs, const uint32_t den, const LUT& lut, const LUT& invLut);

void transform_360(const Transform360Support& t360, uint32_t* out, uint32_t* ibuf1, int width, int height, int start_scanline, int num_scanlines, double yaw, double pitch, double roll, int interpolation);
void transform_360(const Transform360Support& t360, uint32_t* out, uint32_t* ibuf1, int width, int height, int start_scanline, int num_scanlines, const Matrix3& xform, int interpolation);
void transform_360_map(const Transform360Support& t360, float* out, int width, int height, int start_scanline, int num_scanlines, double yaw, double pitch, double roll);
void apply_360_map(uint32_t* out, uint32_t* ibuf1, float* map, int width, int height, int start_scanline, int num_scanlines, int interpolation);



#endif