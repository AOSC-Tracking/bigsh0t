/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MATH_HPP
#define MATH_HPP

#include <vector>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#define DEG2RADF(x) ((x) * M_PI / 180.0)

void smooth (std::vector<double>& samples, int window, double windowBias);

#endif
