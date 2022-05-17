/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>
#include <mutex>
#include "frei0r.hpp"
#include "Math.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "Graphics.hpp"
#include "ImageProcessing.hpp"
#include "Frei0rParameter.hpp"
#include "Frei0rFilter.hpp"
#include "MP4.hpp"
#include "Version.hpp"

#define SEQ_HEMI_TO_EQUIRECT_PROJECTION_EQUIDISTANT_FISHEYE 1
#define INTERP_NONE 0


class ZenithCorrection : public Frei0rFilter, MPFilter {

  public:
    Frei0rParameter<double,double> yaw;
    Frei0rParameter<double,double> pitch;
    Frei0rParameter<double,double> roll;
    Frei0rParameter<int,double> interpolation;

    bool enableSmoothYaw;
    Frei0rParameter<int,double> smoothYaw;
    Frei0rParameter<double,double> timeBiasYaw;

    std::string analysisFile;
    Frei0rParameter<double,double> clipOffset;

    std::mutex lock;

    std::vector<Quaternion> zenithData;
    std::vector<double> yawCorrection;
    std::string zenithDataFrom;
    double frameRate;
    Transform360Support t360;

    ZenithCorrection(unsigned int width, unsigned int height) : Frei0rFilter(width, height), t360(width, height) {
        enableSmoothYaw = false;
        timeBiasYaw = 0.0;
        smoothYaw = 120;
        clipOffset = 0.0;

        analysisFile = std::string("");
        zenithDataFrom = std::string("");

        interpolation = Interpolation::BILINEAR;

        register_param(analysisFile, "analysisFile", "");
        register_fparam(clipOffset, "clipOffset", "");

        register_param(enableSmoothYaw, "enableSmoothYaw", "");
        register_fparam(smoothYaw, "smoothYaw", "");
        register_fparam(timeBiasYaw, "timeBiasYaw", "");

        register_fparam(interpolation, "interpolation", "");
    }

    ~ZenithCorrection() {
    }

    std::string parseFileName (const std::string& fileName) {
        if (fileName.length() > 8 && fileName.compare(0, 8, std::string("file:///")) == 0) {
            if (fileName.length() > 10 && fileName.at (9) == ':') {
                // Windows file URL - file:///X:/...
                return fileName.substr (8);
            } else {
                // UNIX / OSX file URL - file:///home/...
                return fileName.substr (7);
            }
        } else {
            return fileName;
        }
    }

    void loadData() {
        if (analysisFile == zenithDataFrom) {
            return;
        }

        zenithData.clear();
        if (analysisFile == std::string("")) {
            return;
        }

        zenithDataFrom = analysisFile;
        MP4Parser parser(parseFileName(analysisFile));
        if (parser.valid()) {
            float duration = parser.getDuration();
            if (duration > 0) {
                parser.readZenithData(zenithData);
                frameRate = zenithData.size() / duration;
            }
        }
        parser.close();
    }

    void createYawCorrection() {
        yawCorrection.clear();
        yawCorrection.push_back(0.0);

        double acc = 0.0;
        for (int i = 1; i < zenithData.size(); ++i) {
            Quaternion prev;
            invertQ(zenithData[i -1], prev);
            Quaternion current = Quaternion(zenithData[i]);

            Quaternion delta;
            mulQQ(current, prev, delta);

            Quaternion swing;
            Quaternion twist;
            Vector3 up;
            up[0] = 0;
            up[1] = 0;
            up[2] = 1;
            decomposeQ(delta, up, swing, twist);

            Vector3 ahead;
            ahead[0] = 1;
            ahead[1] = 0;
            ahead[2] = 0;

            Matrix3 twistM;
            twistM.identity();
            rotateQuaternion(twistM, twist);

            Vector3 deltaYaw;
            mulM3V3(twistM, ahead, deltaYaw);

            double a = atan2(deltaYaw[1], deltaYaw[0]);

            acc += a;

            yawCorrection.push_back(acc);
        }

        smooth(yawCorrection, smoothYaw, timeBiasYaw / 100.0);
    }

    virtual void update(double time,
                        uint32_t* out,
                        const uint32_t* in) {
        // frei0r filter instances are not thread-safe. Shotcut ignores that, so we'll
        // deal with it by wrapping the execution in a mutex
        std::lock_guard<std::mutex> guard(lock);

        loadData();

        if (enableSmoothYaw) {
            createYawCorrection();
        } else {
            yawCorrection.clear();
        }

        MPFilter::updateMP(this, time, out, in, width, height);

        /*
        double clipTime = time + clipOffset;
        int frame = (int) round(clipTime * frameRate);

        char buf[1024];
        snprintf (buf, 1024,
        "F %d FR %f ZD %d YC %d SY %s\n",
        frame, frameRate, zenithData.size(), yawCorrection.size(),
        enableSmoothYaw ? "ON" : "OFF"
        );
        std::string status(buf);

        Graphics g(out, width, height);
        g.drawText(8, 8, status, 0, 0xff0000ff);
        */
    }

    virtual void updateLines(double time,
                             uint32_t* out,
                             const uint32_t* in, int start, int num) {
        Matrix3 xform;
        xform.identity();
        double clipTime = time + clipOffset;
        int frame = (int) round(clipTime * frameRate);
        if (frame >= 0 && frame < yawCorrection.size()) {
            rotateZ(xform, yawCorrection[frame]);
        }
        if (frame >= 0 && frame < zenithData.size()) {
            Quaternion q;
            invertQ(zenithData[frame], q);
            rotateQuaternion(xform, q);
        }

        transform_360(t360, out, (uint32_t*) in, width, height, start, num, xform, interpolation);
    }
};

frei0r::construct<ZenithCorrection> plugin("bigsh0t_zenith_correction",
        "Applies video zenith correction data.",
        "Leo Sutic <leo@sutic.nu>",
        BIGSH0T_VERSION_MAJOR, BIGSH0T_VERSION_MINOR, F0R_COLOR_MODEL_PACKED32);
