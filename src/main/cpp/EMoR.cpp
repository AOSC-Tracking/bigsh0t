/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <vector>
#include <iostream>
#include "EMoR.hpp"

#include "EMoRData.inc"

EMoR::EMoR() : LUT(), lookup() {
}

EMoR::EMoR(const std::vector<double>& parameters, int inputShift, int outputScale) : LUT(), lookup(), lookupInt(), inputShift(inputShift), outputScale(outputScale) {
    compute(parameters, inputShift, outputScale);
}

EMoR& EMoR::compute(const std::vector<double>& parameters, int inputShift, int outputScale) {
    this->inputShift = inputShift - 10;
    this->outputScale = outputScale;
    lookup.clear();
    for (int i = 0; i < 1024; ++i) {
        double v = EMoR_f0[i];
        for (int j = 0; j < parameters.size(); j++) {
            v += parameters[j] * EMoR_h[j][i];
        }
        lookup.push_back(v);
    }
    makeMonotone();
    return *this;
}

EMoR::~EMoR() {
}

double EMoR::sampleFloat(int pos) const {
    if (pos < 0) {
        pos = 0;
    }
    if (pos > 1023) {
        pos = 1023;
    }
    return lookup[pos];
}

int32_t EMoR::sampleInt(int32_t pos) const {
    if (inputShift > 0) {
        pos >>= inputShift;
    } else {
        pos <<= (-inputShift);
    }

    if (pos < 0) {
        pos = 0;
    }
    if (pos > 1023) {
        pos = 1023;
    }
    return lookupInt[pos];
}

EMoR& EMoR::invert() {
    std::vector<double> newLookup;

    for (int i = 0; i < 1024; ++i) {
        double y = i / 1023.0;
        double x = 1.0;
        for (int j = 1; j < 1023; ++j) {
            if (lookup[j] >= y) {
                x = j / 1023.0;
                break;
            }
        }
        newLookup.push_back(x);
    }
    lookup = newLookup;
    return *this;
}

EMoR& EMoR::makeMonotone() {
    if (lookup[1023] > 1.0) {
        lookup[1023] = 1.0;
    }
    if (lookup[1023] < 0) {
        lookup[1023] = 0;
    }
    for (int i = 1022; i >= 0; --i) {
        if (lookup[i] > lookup[i + 1]) {
            lookup[i] = lookup[i + 1];
        }
        if (lookup[i] < 0) {
            lookup[i] = 0;
        }
    }
    return *this;
}

EMoR& EMoR::initialize() {
    lookupInt.clear();
    for (int i = 0; i < 1024; ++i) {
        double v = lookup[i];
        lookupInt.push_back((int32_t) (v * outputScale));
    }
    return *this;
}