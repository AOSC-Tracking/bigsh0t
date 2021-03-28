/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef ImageProcessing_HPP
#define ImageProcessing_HPP

#include <inttypes.h>
#include "Matrix.hpp"

enum Interpolation {
    NONE = 0,
    BILINEAR = 1
};

uint32_t sampleNearestNeighbor (const uint32_t* frame, double x, double y, int width, int height);
uint32_t sampleBilinear (const uint32_t* frame, double x, double y, int width, int height);
uint32_t sampleBilinearWrappedClamped (const uint32_t* frame, double x, double y, int width, int height);
#ifdef USE_SSE
uint32_t sseBlerp(const uint32_t* frame, int ai, int bi, int ci, int di, int ax, int ay, int width, int height);
#endif
uint32_t int64Blerp(const uint32_t* frame, int ai, int bi, int ci, int di, int ax, int ay, int width, int height);

void transform_360(uint32_t* out, uint32_t* ibuf1, int width, int height, int start_scanline, int num_scanlines, double yaw, double pitch, double roll, int interpolation);
void transform_360(uint32_t* out, uint32_t* ibuf1, int width, int height, int start_scanline, int num_scanlines, const Matrix3& xform, int interpolation);
void transform_360_map(float* out, int width, int height, int start_scanline, int num_scanlines, double yaw, double pitch, double roll);
void apply_360_map(uint32_t* out, uint32_t* ibuf1, float* map, int width, int height, int start_scanline, int num_scanlines, int interpolation);



#endif