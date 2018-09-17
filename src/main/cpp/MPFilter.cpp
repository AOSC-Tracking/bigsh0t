/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "MPFilter.hpp"
#include "omp_compat.h"

void MPFilter::updateMP(MPFilter* filter, double time,
	                  uint32_t* out,
                      const uint32_t* in, int width, int height) {
    int numThreads = omp_get_max_threads();
    int blockSize = (height / numThreads) + 1;
    if (blockSize < 1) {
        blockSize = 1;
    }
    #pragma omp parallel for
    for (int i = 0; i < numThreads; ++i) {
        int start = i * blockSize;
        int end = start + blockSize;
        if (start <= height) {
            if (end > height) {
                end = height;
            }
            filter->updateLines(time, out, in, start, blockSize);
        }
    }
}