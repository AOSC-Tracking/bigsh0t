/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <climits>
#include <cmath>

#include "Math.hpp"

void smooth (std::vector<double>& samples, int window, double windowBias) {
    if (window < 1) {
        window = 1;
    }
    
    double acc = 0.0;
    std::vector<double> sums;
    for (double& v : samples) {
        acc += v;
        sums.push_back(acc);
    }
    
    windowBias = (windowBias + 1.0) / 2;
    
    for (int i = 0; i < samples.size(); ++i) {
        int start = i - window + (int) (windowBias * window) - 1;
        int end = start + window;
        if (start < -1) {
            start = -1;
        }
        if (end >= samples.size()) {
            end = samples.size() - 1;
        }
        int num = end - start;
        double v0 = start < 0 ? 0 : sums[start];
        double v1 = sums[end];
        
        double v = (v1 - v0) / num;
        samples[i] = v;
    }
}

