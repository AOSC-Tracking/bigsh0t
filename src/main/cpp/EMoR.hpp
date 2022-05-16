/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef EMOR_HPP
#define EMOR_HPP

#include <vector>
#include "LUT.hpp"

class EMoR : public LUT {
  public:
    EMoR();
    EMoR(const std::vector<double>& parameters, int inputShift, int outputScale);
    ~EMoR();
    EMoR& compute(const std::vector<double>& parameters, int inputShift, int outputScale);
    EMoR& makeMonotone();
    EMoR& invert();
    EMoR& initialize();
    virtual double sampleFloat(int pos) const;
    virtual int32_t sampleInt(int32_t pos) const;

  private:
    int inputShift;
    int outputScale;
    std::vector<double> lookup;
    std::vector<int32_t> lookupInt;
};

#endif EMOR_HPP