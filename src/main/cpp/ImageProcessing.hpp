#ifndef ImageProcessing_HPP
#define ImageProcessing_HPP

#include <inttypes.h>

enum Interpolation {
    NONE = 0,
    BILINEAR = 1
};

uint32_t sampleNearestNeighbor (uint32_t* frame, double x, double y, int width, int height);
uint32_t sampleBilinear (uint32_t* frame, double x, double y, int width, int height);
uint32_t sampleBilinearWrappedClamped (uint32_t* frame, double x, double y, int width, int height);

#endif