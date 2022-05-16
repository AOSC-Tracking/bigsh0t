/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef LUT_HPP
#define LUT_HPP

class LUT {
  public:
    virtual double sampleFloat(int pos) const = 0;
    virtual int32_t sampleInt(int32_t pos) const = 0;
};

#endif EMOR_HPP