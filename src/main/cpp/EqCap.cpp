/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>
#include <cstring>
#include <mutex>

#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "Frei0rParameter.hpp"
#include "Frei0rFilter.hpp"
#include "Math.hpp"
#include "ImageProcessing.hpp"
#include "SummedAreaTable.hpp"

class CapParameters {
  public:
    Frei0rParameter<double,double> start;
    Frei0rParameter<double,double> end;
    Frei0rParameter<double,double> blendIn;
    Frei0rParameter<double,double> blendOut;
    Frei0rParameter<double,double> fadeIn;
    Frei0rParameter<double,double> blurWidthStart;
    Frei0rParameter<double,double> blurWidthEnd;
    Frei0rParameter<double,double> blurHeightStart;
    Frei0rParameter<double,double> blurHeightEnd;
    bool enabled;
    const bool isBottom;

    int startPixels;
    int endPixels;
    int blendInPixels;
    int blendOutPixels;
    int fadeInPixels;
    int blurWidthStartPixels;
    int blurWidthEndPixels;
    int blurHeightStartPixels;
    int blurHeightEndPixels;
    int minSampleRow;
    int maxSampleRow;
    SummedAreaTable sat;

    CapParameters(bool _isBottom) : isBottom(_isBottom), sat(10, 10) {
        start = 45;
        end = 85;
        blendIn = 0.0;
        blendOut = 10.0;
        blurWidthStart = 0.0;
        blurWidthEnd = 360.0;
        blurHeightStart = 0.0;
        blurHeightEnd = 2.0;
        fadeIn = 10.0;
        enabled = true;
    }

    void compute(int width, int height, const uint32_t* in) {
        double yawToPixels = width / 360.0;
        double pitchToPixels = height / 180.0;
        int height2 = height >> 1;
        int width2 = width >> 1;
        startPixels = height2 - start * pitchToPixels;
        if (startPixels < 1) {
            startPixels = 1;
        }
        if (startPixels > height2) {
            startPixels = height2;
        }
        endPixels = height2 - end * pitchToPixels;
        if (endPixels < 0) {
            endPixels = 0;
        }
        if (endPixels >= startPixels) {
            endPixels = startPixels - 1;
        }
        blendInPixels = blendIn * pitchToPixels;
        if (blendInPixels < 0) {
            blendInPixels = 0;
        }
        if (blendInPixels > (height2 - startPixels)) {
            blendInPixels = height2 - startPixels;
        }
        blendOutPixels = blendOut * pitchToPixels;
        if (blendOutPixels < 0) {
            blendOutPixels = 0;
        }
        if (blendOutPixels > (startPixels - endPixels)) {
            blendOutPixels = (startPixels - endPixels);
        }

        fadeInPixels = fadeIn * pitchToPixels;

        blurWidthStartPixels = blurWidthStart * yawToPixels;
        if (blurWidthStartPixels < width) {
            ++blurWidthStartPixels;
        }
        blurWidthEndPixels = blurWidthEnd * yawToPixels;
        if (blurWidthEndPixels < width) {
            ++blurWidthEndPixels;
        }
        blurHeightStartPixels = blurHeightStart * pitchToPixels + 1;
        blurHeightEndPixels = blurHeightEnd * pitchToPixels + 1;

        if (!isBottom) {
            minSampleRow = startPixels - blendOutPixels - blurHeightEndPixels - 1;
            maxSampleRow = startPixels + blendInPixels + blurHeightEndPixels + 1;
        } else {
            minSampleRow = height - startPixels - blendInPixels - blurHeightEndPixels - 1;
            maxSampleRow = height - startPixels + blendOutPixels + blurHeightEndPixels + 1;
        }
        if (minSampleRow < 0) {
            minSampleRow = 0;
        }
        if (minSampleRow > height) {
            minSampleRow = height;
        }
        if (maxSampleRow < 0) {
            maxSampleRow = 0;
        }
        if (maxSampleRow > height) {
            maxSampleRow = height;
        }
        sat.compute(in, width, 0, minSampleRow, width, maxSampleRow - minSampleRow);
    }
};

class EqCap : public Frei0rFilter, MPFilter {

  public:
    CapParameters top;
    CapParameters bottom;
    Frei0rParameter<int,double> interpolation;
    Transform360Support t360;

    std::mutex lock;

    EqCap(unsigned int width, unsigned int height) : Frei0rFilter(width, height), top(false), bottom(true), t360(width, height) {
        interpolation = 0;

        register_param(top.enabled, "topEnabled", "");
        register_fparam(top.start, "topStart", "");
        register_fparam(top.end, "topEnd", "");
        register_fparam(top.blendIn, "topBlendIn", "");
        register_fparam(top.blendOut, "topBlendOut", "");
        register_fparam(top.fadeIn, "topFadeIn", "");
        register_fparam(top.blurWidthStart, "topBlurWidthStart", "");
        register_fparam(top.blurWidthEnd, "topBlurWidthEnd", "");
        register_fparam(top.blurHeightStart, "topBlurHeightStart", "");
        register_fparam(top.blurHeightEnd, "topBlurHeightEnd", "");

        register_param(bottom.enabled, "bottomEnabled", "");
        register_fparam(bottom.start, "bottomStart", "");
        register_fparam(bottom.end, "bottomEnd", "");
        register_fparam(bottom.blendIn, "bottomBlendIn", "");
        register_fparam(bottom.blendOut, "bottomBlendOut", "");
        register_fparam(bottom.fadeIn, "bottomFadeIn", "");
        register_fparam(bottom.blurWidthStart, "bottomBlurWidthStart", "");
        register_fparam(bottom.blurWidthEnd, "bottomBlurWidthEnd", "");
        register_fparam(bottom.blurHeightStart, "bottomBlurHeightStart", "");
        register_fparam(bottom.blurHeightEnd, "bottomBlurHeightEnd", "");

        register_fparam(interpolation, "interpolation", "");
    }

    ~EqCap() {
    }

    virtual void update(double time,
                        uint32_t* out,
                        const uint32_t* in) {

        // frei0r filter instances are not thread-safe. Shotcut ignores that, so we'll
        // deal with it by wrapping the execution in a mutex
        std::lock_guard<std::mutex> guard(lock);

        top.compute(width, height, in);
        bottom.compute(width, height, in);

        MPFilter::updateMP(this, time, out, in, width, height);
    }

    virtual void updateLines(double time,
                             uint32_t* out,
                             const uint32_t* in,
                             int start, int num) {
        for (int y = start; y < start + num; ++y) {
            CapParameters& hemisphere = (y < height / 2) ? top : bottom;
            int hy = y;
            int h0 = 0;
            int hm = 1;
            int isBottom = 0;
            if (hemisphere.isBottom) {
                hy = height - 1 - y;
                h0 = height - 1;
                hm = -1;
                isBottom = 1;
            }
            if (hemisphere.enabled && hy < hemisphere.endPixels) {
                int sampleWidth = hemisphere.blurWidthEndPixels;
                int sampleHeight = hemisphere.blurHeightEndPixels;
                int sampleY = h0 + hm * (hemisphere.startPixels - hemisphere.blendOutPixels - sampleHeight / 2);
                int sampleX = - sampleWidth / 2;
                sampleY -= sampleHeight * isBottom;
                sampleY -= hemisphere.minSampleRow;
                uint32_t* p = &out[y * width];
                for (int x = 0; x < width; ++x) {
                    *p = hemisphere.sat.averagePixel(sampleX, sampleY, sampleWidth, sampleHeight);
                    ++sampleX;
                    ++p;
                }
            } else if (hemisphere.enabled && hy < hemisphere.startPixels) {
                float amount = 1.0f - ((float)(hy - hemisphere.endPixels)) / (hemisphere.startPixels - hemisphere.endPixels);
                int sampleWidth = hemisphere.blurWidthEndPixels * amount + (1.0f - amount) * hemisphere.blurWidthStartPixels;
                int sampleHeight = hemisphere.blurHeightEndPixels * amount + (1.0f - amount) * hemisphere.blurHeightStartPixels;
                int sampleX = - sampleWidth / 2;
                int sampleY = h0 + hm * (hemisphere.startPixels + hemisphere.blendInPixels - amount * (hemisphere.blendOutPixels + hemisphere.blendInPixels) - sampleHeight / 2);
                sampleY -= sampleHeight * isBottom;
                sampleY -= hemisphere.minSampleRow;
                uint32_t* p = &out[y * width];

                if (hy < hemisphere.startPixels - hemisphere.fadeInPixels) {
                    for (int x = 0; x < width; ++x) {
                        *p = hemisphere.sat.averagePixel(sampleX, sampleY, sampleWidth, sampleHeight);
                        ++sampleX;
                        ++p;
                    }
                } else {
                    int fade = ((hy - (hemisphere.startPixels - hemisphere.fadeInPixels)) << 7) / hemisphere.fadeInPixels;
                    const uint32_t* s = &in[y * width];
                    for (int x = 0; x < width; ++x) {
                        *p = int64lerp(
                                 hemisphere.sat.averagePixel(sampleX, sampleY, sampleWidth, sampleHeight),
                                 *s,
                                 fade);
                        ++sampleX;
                        ++p;
                        ++s;
                    }
                }
            } else {
                uint32_t* p = &out[y * width];
                const uint32_t* s = &in[y * width];
                memcpy(p, s, width * sizeof(uint32_t));
            }
        }
    }

  protected:

  private:

};

frei0r::construct<EqCap> plugin("eq_cap",
                                "Masks part of the VR sphere.",
                                "Leo Sutic <leo@sutic.nu>",
                                2, 6, F0R_COLOR_MODEL_RGBA8888);
