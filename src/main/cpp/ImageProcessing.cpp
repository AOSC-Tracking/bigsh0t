#include "ImageProcessing.hpp"
#include "frei0r_math.h"

uint32_t sampleNearestNeighbor (uint32_t* frame, double x, double y, int width, int height) {
    return frame[((int) y) * width + ((int) x)];
}

uint32_t lerp(uint32_t a, uint32_t b, int x) {
    int c0a = (a >> 24) & 0xff;
    int c1a = (a >> 16) & 0xff;
    int c2a = (a >>  8) & 0xff;
    int c3a = (a      ) & 0xff;
    
    int c0b = (b >> 24) & 0xff;
    int c1b = (b >> 16) & 0xff;
    int c2b = (b >>  8) & 0xff;
    int c3b = (b      ) & 0xff;
    
    int t = 0;
    
    int c0c = INT_BLEND(c0b, c0a, x, t);
    int c1c = INT_BLEND(c1b, c1a, x, t);
    int c2c = INT_BLEND(c2b, c2a, x, t);
    int c3c = INT_BLEND(c3b, c3a, x, t);
    
    return (c0c << 24) | (c1c << 16) | (c2c << 8) | c3c;
}

uint32_t sampleBilinear (uint32_t* frame, double x, double y, int width, int height) {
    int ix0 = x;
    int iy0 = y;
    int ix1 = ix0 + 1;
    int iy1 = iy0 + 1;
    int ax = (x - ix0) * 256;
    int ay = (y - iy0) * 256;
    
    if (ix0 < 0 || iy0 < 0 || ix0 > width - 1 || iy0 > height - 1) {
        return 0;
    }
    if (ix1 >= width) {
        ix1 = width - 1;
    }
    if (iy1 >= height) {
        iy1 = height - 1;
    }
    
    uint32_t a = frame[iy0 * width + ix0];
    uint32_t b = frame[iy0 * width + ix1];
    uint32_t c = frame[iy1 * width + ix0];
    uint32_t d = frame[iy1 * width + ix1];
    uint32_t t = 0;
    
    
    
    uint32_t e = lerp(a, b, ax);
    uint32_t f = lerp(c, d, ax);
    return lerp(e, f, ay);
}

uint32_t sampleBilinearWrappedClamped (uint32_t* frame, double x, double y, int width, int height) {
    if (x < 0) {
        x += width;
    }
    if (x >= width) {
        x -= width;
    }
    if (y < 0) {
        y = 0;
    }
    if (y >= height) {
        y = height;
    }
    
    int ix0 = x;
    int iy0 = y;
    int ix1 = ix0 + 1;
    int iy1 = iy0 + 1;
    int ax = (x - ix0) * 256;
    int ay = (y - iy0) * 256;
    
    
    if (ix1 >= width) {
        ix1 -= width;
    }
    if (iy1 >= height) {
        iy1 = height - 1;
    }
    
    uint32_t a = frame[iy0 * width + ix0];
    uint32_t b = frame[iy0 * width + ix1];
    uint32_t c = frame[iy1 * width + ix0];
    uint32_t d = frame[iy1 * width + ix1];
    uint32_t t = 0;
    
    uint32_t e = lerp(a, b, ax);
    uint32_t f = lerp(c, d, ax);
    return lerp(e, f, ay);
}