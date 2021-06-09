/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <climits>
#include <cmath>
#include <algorithm>
#include <cfloat>

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

/**
 * Fast implementation of the arctan2 function.
 *
 * Based on "A Sequence of Polynomials for Approximating Arctangent"
 * by Herbert A. Medina, Loyola Marymount University, hmedina@lmu.edu.
 * https://digitalcommons.lmu.edu/cgi/viewcontent.cgi?article=1101&context=math_fac
 * Worst error is 2.28e-07 radians, which means that you'd need an equirectangular
 * map 215296 pixels wide before you are off by 1/128th of a pixel, which is the
 * resolution of the bilinear interpolation code.
 */
double fastAtan2(double y, double x) {
    double abs_x = std::abs(x);
    double abs_y = std::abs(y);
    if (abs_x < DBL_MIN && abs_y < DBL_MIN) {
        return 0.0;
    }
    double a = std::min(abs_x, abs_y) / std::max(abs_x, abs_y);

    double a2 = a * a;

    double r = 0 + a * (1.0 + a2*((-1.0/3.0) + a2*((1.0/5.0) + a2*((-1.0/7.0) + a2*((5.0/48.0) + a*((1.0/20.0) + a*((-43.0/176.0) + a*((1.0/4.0) + a*((-27.0/208) + a*((1.0/28.0) + a*((-1.0/240.0))))))))))));

    if (abs_y > abs_x) {
        r = (M_PI / 2) - r;
    }
    if (x < 0) {
        r = M_PI - r;
    }
    if (y < 0) {
        r = -r;
    }
    return r;
}

