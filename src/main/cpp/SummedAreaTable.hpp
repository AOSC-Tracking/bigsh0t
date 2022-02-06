/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef SummedAreaTable_HPP
#define SummedAreaTable_HPP

#include <inttypes.h>

class SummedAreaTable {
  public:
    SummedAreaTable(int width, int height);
    ~SummedAreaTable();

    void compute(const uint32_t* in, int inW, int x0, int y0, int w, int h);
    void dump();
    uint32_t averagePixel(int x0, int y0, int sampleWidth, int sampleHeight);
    void sumComponents(int imx0, int imy0, int sampleWidth, int sampleHeight, uint32_t& r, uint32_t& g, uint32_t& b, uint32_t& a);
    uint32_t sum(int imx0, int imy0, int sampleWidth, int sampleHeight);

  private:
    int width;
    int height;
    uint32_t* sums;


    uint32_t sampleComponent(int x0, int y0, int x1, int y1, int component);
};

#endif