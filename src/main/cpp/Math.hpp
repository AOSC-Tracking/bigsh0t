/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MATH_HPP
#define MATH_HPP

#include <vector>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#ifndef M_PI_R
#define M_PI_R           (1.0 / M_PI)
#endif


#define DEG2RADF(x) ((x) * M_PI / 180.0)

void smooth (std::vector<double>& samples, int window, double windowBias);
double fastAtan2(double y, double x);

#endif
