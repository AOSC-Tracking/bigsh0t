/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include "ImageProcessing.hpp"
#include "SummedAreaTable.hpp"

SummedAreaTable::SummedAreaTable(int width, int height) {
    this->width = width + 1;
    this->height = height + 1;
    this->sums = (uint32_t*) malloc(this->width * this->height * sizeof(uint32_t) * 4);
}

SummedAreaTable::~SummedAreaTable() {
    free(sums);
}

void SummedAreaTable::dump() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int i = y * width * 4 + x * 4;
            std::cout << sums[i] << ";" << sums[i + 1] << ";" << sums[i + 2] << ";" << sums[i + 3] << ", ";
        }
        std::cout << std::endl;
    }
}

void SummedAreaTable::compute(const uint32_t *in, int inW, int x0, int y0, int w, int h) {
    if (width != (w + 1) || height != (h + 1)) {
        free(sums);
        width = w + 1;
        height = h + 1;
        sums = (uint32_t*) malloc(this->width * this->height * sizeof(uint32_t) * 4);
    }
    const int stride = width * 4;

    for (int y = 0; y < height; ++y) {
        sums[y * width * 4 + 0] = 0;
        sums[y * width * 4 + 1] = 0;
        sums[y * width * 4 + 2] = 0;
        sums[y * width * 4 + 3] = 0;
    }
    for (int x = 0; x < width * 4; ++x) {
        sums[x] = 0;
    }
    #pragma omp parallel for
    for (int c = 0; c < 4; ++c) {
        uint32_t* p = &sums[stride + 4 + c];
        int shift = c * 8;
        for (int y = 1; y < height; ++y) {
            const uint32_t* s = &in[(y0 + y - 1) * (inW) + x0];
            for (int x = 1; x < width; ++x) {
                uint32_t v = ((*s) >> shift) & 0xff;
                if (y == 1 && x == 1) {
                    *p = v;
                } else if (y == 1) {
                    *p = v + *(p - 4);
                } else if (x == 1) {
                    *p = v + *(p - stride);
                } else {
                    *p = v + *(p - stride) + *(p - 4) - *(p - stride - 4);
                }
                p += 4;
                ++s;
            }
            p += 4;
        }
    }
}

void SummedAreaTable::sumComponents(int imx0, int imy0, int sampleWidth, int sampleHeight, uint32_t& r, uint32_t& g, uint32_t& b, uint32_t& a) {
    if (imx0 < 0) {
        imx0 += width -1;
    }

    int x0 = imx0;
    int y0 = imy0;

    int x1 = x0 + sampleWidth;
    int y1 = y0 + sampleHeight;
    if (x1 < width) {
        r = sampleComponent(x0, y0, x1, y1, 0);
        g = sampleComponent(x0, y0, x1, y1, 1);
        b = sampleComponent(x0, y0, x1, y1, 2);
        a = sampleComponent(x0, y0, x1, y1, 3);
    } else {
        int w0 = (width - 1) - x0;
        x1 = x1 - (width - 1);

        r = sampleComponent(x0, y0, width - 1, y1, 0);
        g = sampleComponent(x0, y0, width - 1, y1, 1);
        b = sampleComponent(x0, y0, width - 1, y1, 2);
        a = sampleComponent(x0, y0, width - 1, y1, 3);

        r += sampleComponent(0, y0, x1, y1, 0);
        g += sampleComponent(0, y0, x1, y1, 1);
        b += sampleComponent(0, y0, x1, y1, 2);
        a += sampleComponent(0, y0, x1, y1, 3);

    }
}

uint32_t SummedAreaTable::averagePixel(int x0, int y0, int w, int h) {
    uint32_t r = 0;
    uint32_t g = 0;
    uint32_t b = 0;
    uint32_t a = 0;
    int area = w * h;
    sumComponents(x0, y0, w, h, r, g, b, a);
    r /= area;
    g /= area;
    b /= area;
    a /= area;
    return (a << 24) | (b << 16) | (g << 8) | r;
}

uint32_t SummedAreaTable::sum(int x0, int y0, int w, int h) {
    uint32_t r = 0;
    uint32_t g = 0;
    uint32_t b = 0;
    uint32_t a = 0;
    int area = w * h;
    sumComponents(x0, y0, w, h, r, g, b, a);
    return r + g + b + a;
}

uint32_t SummedAreaTable::sampleComponent(int x0, int y0, int x1, int y1, int component) {
    int iy0 = y0 * width;
    int iy1 = y1 * width;
    int ixA = iy0 + x0;
    int ixB = iy0 + x1;
    int ixC = iy1 + x0;
    int ixD = iy1 + x1;
    int A = sums[(ixA << 2) + component];
    int B = sums[(ixB << 2) + component];
    int C = sums[(ixC << 2) + component];
    int D = sums[(ixD << 2) + component];

    return D - B - C + A;
}