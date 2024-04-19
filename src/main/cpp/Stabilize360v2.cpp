/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>
#include <vector>
#include <string>
#include <cstring>
#include <mutex>
#include <fstream>
#include <algorithm>
#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "Graphics.hpp"
#include "ImageProcessing.hpp"
#include "Frei0rParameter.hpp"
#include "Frei0rFilter.hpp"
#include "omp_compat.h"
#include "Version.hpp"
#include "Debug.hpp"

#define ROTATION_TIME_INSTANT (1.0 / 10000.0)


class Rotation {

  public:
    double previousTime;
    double time;
    double yaw;
    double pitch;
    double roll;
    bool updated;

    Rotation (double previousTime, double time, double yaw, double pitch, double roll, bool updated) {
        this->previousTime = previousTime;
        this->time = time;
        this->yaw = yaw;
        this->pitch = pitch;
        this->roll = roll;
        this->updated = updated;
    }

    ~Rotation() {
    }

    double span () const {
        return time - previousTime;
    }

    bool contains (const double t) const {
        return t >= previousTime && t <= time;
    }

    bool spanOverlaps (const Rotation& other) const {
        if (other.contains(time) || other.contains (previousTime)) {
            return true;
        }
        if (previousTime < other.previousTime && time > other.time) {
            return true;
        }
        return false;
    }

    void writeTo (std::ofstream& file) {
        file.write ((char*) &previousTime, sizeof(previousTime));
        file.write ((char*) &time, sizeof(time));
        file.write ((char*) &yaw, sizeof(yaw));
        file.write ((char*) &pitch, sizeof(pitch));
        file.write ((char*) &roll, sizeof(roll));
    }

    void readFrom (std::ifstream& file) {
        file.read ((char*) &previousTime, sizeof(previousTime));
        file.read ((char*) &time, sizeof(time));
        file.read ((char*) &yaw, sizeof(yaw));
        file.read ((char*) &pitch, sizeof(pitch));
        file.read ((char*) &roll, sizeof(roll));
        this->updated = false;
    }
};

class RotationSamples {
  public:
    std::vector<Rotation> rotations;
    double minSpan;

    void add (Rotation rot) {
        removeOverlapping (rot);
        if (rotations.size() == 0) {
            rotations.push_back(rot);
            return;
        }
        size_t pos = 0;
        for (size_t i = rotations.size(); i > 0; --i) {
            if (rotations[i - 1].time < rot.time) {
                pos = i;
                break;
            }
        }
        if (pos == rotations.size()) {
            rotations.push_back(rot);
        } else {
            rotations.insert (rotations.begin() + pos, rot);
        }

        updateMinSpan();
    }

    Rotation getMax () {
        Rotation m (0, 0, 0, 0, 0, false);
        for (Rotation& r : rotations) {
            double ay = std::abs(r.yaw);
            double ap = std::abs(r.pitch);
            double ar = std::abs(r.roll);
            if (ay > m.yaw) {
                m.yaw = ay;
            }
            if (ap > m.pitch) {
                m.pitch = ap;
            }
            if (ar > m.roll) {
                m.roll = ar;
            }
        }
        m.time = m.yaw;
        if (m.pitch > m.time) {
            m.time = m.pitch;
        }
        if (m.roll > m.time) {
            m.time = m.roll;
        }
        return m;
    }

    void drawDiagramValue (Graphics& g, int x, int y, int h, uint32_t mask, uint32_t color) {
        int y0 = y;
        if (h < 0) {
            h = -h;
            y0 -= h;
        }
        g.plot (x, y, mask, color);
        g.fillRect (x, y0, 1, h, mask, color);
    }

    void drawDiagram (Graphics& g, double time, int x, int y, int samples, int height) {
        int lastIndex = indexOf (time);
        if (lastIndex == -1) {
            return;
        }
        if (samples > lastIndex + 1) {
            samples = lastIndex + 1;
        }
        int rx = x + (samples / 2);
        double scale = getMax ().time;
        if (scale < 0.01) {
            scale = 1.0;
        }
        for (int i = 0; i < samples; ++i) {
            int index = lastIndex - i;
            Rotation& r = rotations[index];
            int x0 = rx - i;

            drawDiagramValue (g, x0, y, (int) (height * r.yaw / scale),   0x0000ffff, 0xffff0000);
            drawDiagramValue (g, x0, y, (int) (height * r.pitch / scale), 0x00ff00ff, 0xff00ff00);
            drawDiagramValue (g, x0, y, (int) (height * r.roll / scale),  0x00ffff00, 0xff0000ff);
        }
    }

    void clear () {
        rotations.clear();
        updateMinSpan();
    }

    Rotation& operator[](int index) {
        return rotations[index];
    }

    int indexOf (const Rotation& rot) {
        return indexOf (rot.time);
    }

    int indexOf (const double time) {
        for (int i = 0; i < rotations.size(); ++i) {
            Rotation r = rotations[i];
            if (r.contains(time)) {
                return i;
            }
        }
        return -1;
    }

    int lookup(const double time) {
        return indexOf(time - getMinSpan() / 2);
    }

    size_t size () {
        return rotations.size();
    }

    void removeOverlapping (const Rotation& r) {
        for (int i = ((int) rotations.size()) - 1; i >= 0; --i) {
            Rotation r2 = rotations[i];
            if (r.spanOverlaps (r2)) {
                rotations.erase (rotations.begin() + i);
            }
        }
    }

    bool hasOverlapping (const Rotation& r) {
        for (int i = ((int) rotations.size()) - 1; i >= 0; --i) {
            Rotation r2 = rotations[i];
            if (r.spanOverlaps (r2)) {
                return true;
            }
        }
        return false;
    }

    void updateMinSpan() {
        for (Rotation& r : rotations) {
            double s = r.span();
            if (minSpan < 0.0 || s < minSpan) {
                minSpan = s;
            }
        }
    }

    double getMinSpan() {
        return minSpan;
    }

    int findFirstSkip () {
        if (size () < 2) {
            return -1;
        }

        double intervalSum = 0;
        for (int i = 1; i < rotations.size(); ++i) {
            Rotation& r0 = rotations[i - 1];
            Rotation& r1 = rotations[i];
            intervalSum += r1.time - r0.time;
        }
        double averageInterval = intervalSum / (size () - 1);
        for (int i = 1; i < rotations.size(); ++i) {
            Rotation& r0 = rotations[i - 1];
            Rotation& r1 = rotations[i];
            double interval = r1.time - r0.time;
            if (interval > averageInterval * 1.5) {
                return i - 1;
            }
        }
        return -1;
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

    void write (const std::string& fileName) {
        std::ofstream file;
        char buffer [128000];
        file.rdbuf()->pubsetbuf(buffer, 128000);

        file.open (parseFileName(fileName), std::ios::out | std::ios::binary);
        if (file) {
            uint64_t s = (uint64_t) size();
            file.write ((char*) &s, sizeof(s));
            for (Rotation& r : rotations) {
                r.writeTo (file);
            }
            file.close();
        }
    }

    void read (const std::string& fileName) {
        std::ifstream file;
        char buffer [128000];
        file.rdbuf()->pubsetbuf(buffer, 128000);

        file.open (parseFileName(fileName), std::ios::in | std::ios::binary);
        if (file) {
            uint64_t s = 0;
            file.read ((char*) &s, sizeof(s));
            for (int i = 0; i < s; ++i) {
                Rotation r(0, 0, 0, 0, 0, false);
                r.readFrom (file);
                rotations.push_back (r);
            }
            file.close();
        }
        updateMinSpan();
    }

    void smoothComponent (std::vector<double>& samples, int window, double windowBias) {
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
        updateMinSpan();
    }

    void correct (int wYaw, int wPitch, int wRoll, double bYaw, double bPitch, double bRoll, RotationSamples& dest) {
        std::vector<Rotation> accs;
        {
            Rotation acc (0, 0, 0, 0, 0, false);
            for (Rotation& r : rotations) {
                acc.yaw += r.yaw;
                acc.pitch += r.pitch;
                acc.roll += r.roll;

                Rotation r2 (r.previousTime, r.time, acc.yaw, acc.pitch, acc.roll, false);
                accs.push_back(r2);
            }
        }

        std::vector<Rotation> smooths;
        {
            std::vector<double> cYaw;
            std::vector<double> cPitch;
            std::vector<double> cRoll;
            for (Rotation& r : accs) {
                cYaw.push_back(r.yaw);
                cPitch.push_back(r.pitch);
                cRoll.push_back(r.roll);
            }
            smoothComponent (cYaw, wYaw, bYaw);
            smoothComponent (cPitch, wPitch, bPitch);
            smoothComponent (cRoll, wRoll, bRoll);

            for (int i = 0; i < accs.size(); ++i) {
                Rotation& r = accs[i];

                Rotation smooth (r.previousTime, r.time, cYaw[i], cPitch[i], cRoll[i], false);

                smooths.push_back (smooth);
            }
        }

        for (int i = 0; i < accs.size(); ++i) {
            Rotation& measured = accs[i];
            Rotation& smoothed = smooths[i];

            Rotation correction (measured.previousTime, measured.time, measured.yaw - smoothed.yaw, measured.pitch - smoothed.pitch, measured.roll - smoothed.roll, false);
            dest.add(correction);
        }
    }

    void merge (RotationSamples& other) {
        for (size_t i = 0; i < other.rotations.size(); ++i) {
            const Rotation& otherRot = other.rotations[i];
            if (otherRot.updated || !hasOverlapping(otherRot)) {
                add(otherRot);
            }
        }
    }

    void clearUpdated () {
        for (Rotation& r : rotations) {
            r.updated = false;
        }
    }
};

inline short toGray(uint32_t color) {
    return
        ((color      ) & 0xff) +
        ((color >>  8) & 0xff) +
        ((color >> 16) & 0xff);
}


class Stabilize360v2 : public Frei0rFilter, MPFilter {

  private:
    bool previousAnalyzeState;
    bool initializedAnalyzeState;

    double yaw;
    double pitch;
    double roll;

    RotationSamples rawSamples;
    RotationSamples corrections;

    std::mutex lock;
    Transform360Support t360;

    uint32_t* previousFrame;
    double previousFrameTime;

    uint32_t* currentFrame;
    uint32_t* proposalFrame;

    int reducedWidth;
    int reducedHeight;
    int reducedScale;

  public:
    Frei0rParameter<int,double> interpolation;
    bool analyze;

    Frei0rParameter<int,double> maxStep;
    Frei0rParameter<int,double> subpixels;

    double stabilizeYaw;
    double stabilizePitch;
    double stabilizeRoll;

    Frei0rParameter<int,double> smoothYaw;
    Frei0rParameter<int,double> smoothPitch;
    Frei0rParameter<int,double> smoothRoll;

    Frei0rParameter<double,double> timeBiasYaw;
    Frei0rParameter<double,double> timeBiasPitch;
    Frei0rParameter<double,double> timeBiasRoll;

    std::string analysisFile;
    Frei0rParameter<double,double> clipOffset;

    Stabilize360v2(unsigned int width, unsigned int height) : Frei0rFilter(width, height), t360(width, height) {
        initializedAnalyzeState = false;
        previousAnalyzeState = false;

        interpolation = Interpolation::BILINEAR;

        maxStep = 8.0;
        subpixels = 0;

        previousFrame = NULL;
        previousFrameTime = -1;
        analyze = false;

        reducedScale = height / 128;
        if (reducedScale < 1) {
            reducedScale = 1;
        }
        reducedHeight = height / reducedScale;
        reducedWidth = width / reducedScale;
        currentFrame = (uint32_t*) malloc(reducedWidth * reducedHeight * sizeof(uint32_t));
        proposalFrame = (uint32_t*) malloc(reducedWidth * reducedHeight * sizeof(uint32_t));

        yaw = 0;
        pitch = 0;
        roll = 0;
        stabilizeRoll = 1.0;
        stabilizePitch = 1.0;
        stabilizeYaw = 1.0;
        timeBiasYaw = 0.0;
        timeBiasPitch = 0.0;
        timeBiasRoll = 0.0;

        smoothYaw = 120;
        smoothPitch = 120;
        smoothRoll = 120;

        register_param(analysisFile, "analysisFile", "");
        register_fparam(clipOffset, "clipOffset", "");
        register_fparam(interpolation, "interpolation", "");
        register_param(analyze, "analyze", "");
        register_fparam(maxStep, "maxStep", "");
        register_fparam(subpixels, "subpixels", "");

        register_param(stabilizeYaw, "stabilizeYaw", "");
        register_param(stabilizePitch, "stabilizePitch", "");
        register_param(stabilizeRoll, "stabilizeRoll", "");

        register_fparam(smoothYaw, "smoothYaw", "");
        register_fparam(smoothPitch, "smoothPitch", "");
        register_fparam(smoothRoll, "smoothRoll", "");

        register_fparam(timeBiasYaw, "timeBiasYaw", "");
        register_fparam(timeBiasPitch, "timeBiasPitch", "");
        register_fparam(timeBiasRoll, "timeBiasRoll", "");
    }

    virtual ~Stabilize360v2() {
        if (analyze) {
            endAnalyze ();
        } else {
            endApply ();
        }

        if (previousFrame != NULL) {
            free(previousFrame);
            previousFrame = NULL;
        }
        free(currentFrame);
        free(proposalFrame);
    }

    void updateAnalyzeState(double time,
                            uint32_t* out,
                            const uint32_t* in) {
        if (!initializedAnalyzeState) {
            previousAnalyzeState = analyze;
            initializedAnalyzeState = true;
            if (analyze) {
                beginAnalyze (time, out, in);
            } else {
                beginApply (time, out, in);
            }
        } else if (previousAnalyzeState != analyze) {
            previousAnalyzeState = analyze;
            if (analyze) {
                endApply ();
                beginAnalyze (time, out, in);
            } else {
                endAnalyze ();
                beginApply (time, out, in);
            }
        }
    }

    virtual void beginAnalyze(double time, uint32_t* out, const uint32_t* in) {
        rawSamples.clear();
        if (!analysisFile.empty()) {
            rawSamples.read (analysisFile);
        }
    }

    virtual void endAnalyze() {
        if (!analysisFile.empty() && rawSamples.size() > 0) {
            RotationSamples onDisk;
            onDisk.read (analysisFile);
            onDisk.merge (rawSamples);
            onDisk.write (analysisFile);

            rawSamples.clear ();
            rawSamples.read (analysisFile);
        }
    }

    virtual void beginApply(double time, uint32_t* out, const uint32_t* in) {
        rawSamples.clear ();
        if (!analysisFile.empty()) {
            rawSamples.read (analysisFile);
        }
        updateCorrections();
    }

    void updateCorrections() {
        corrections.clear ();
        rawSamples.correct (smoothYaw, smoothPitch, smoothRoll, timeBiasYaw / 100.0, timeBiasPitch / 100.0, timeBiasRoll / 100.0, corrections);
    }

    virtual void endApply() {
    }

    void blt(const uint32_t* in, uint32_t* out, int x0, int y0, int inWidth, int inHeight) {
        for (int x = 0; x < inWidth; ++x) {
            for (int y = 0; y < inHeight; ++y) {
                out[(y + y0) * width + (x + x0)] = in[y * inWidth + x];
            }
        }
    }

    virtual void update(double time, uint32_t* out, const uint32_t* in) {
        // frei0r filter instances are not thread-safe. Shotcut ignores that, so we'll
        // deal with it by wrapping the execution in a mutex
        std::lock_guard<std::mutex> guard(lock);
        double clipTime = time + clipOffset;

        updateAnalyzeState (clipTime, out, in);

        if (analyze) {
            int numSubpixels = 1 << subpixels;

            shrinkAndAccumulate(in, currentFrame, width, height, reducedScale, reducedWidth, reducedHeight);

            memcpy(out, in, width * height * sizeof(uint32_t));

            bool updated = false;
            if (previousFrame != NULL && previousFrameTime < clipTime) {
                updated = true;

                double dy = 0.0;
                double dp = 0.0;
                double dr = 0.0;

                double degreesPerPixel = 360.0 / width;
                double minStep = degreesPerPixel / numSubpixels;
                double step = maxStep;

                int64_t best = diff(previousFrame, currentFrame, reducedWidth, reducedHeight, INT64_MAX);

                Transform360Support t360support (reducedWidth, reducedHeight);

                while (step >= minStep) {
                    int bys = 0;
                    int bps = 0;
                    int brs = 0;
                    for (int ys = -1; ys <= 1; ++ys) {
                        for (int ps = -1; ps <= 1; ++ps) {
                            for (int rs = -1; rs <= 1; ++rs) {
                                if (ys != 0 || ps != 0 || rs != 0) {
                                    double py = dy + ys * step;
                                    double pp = dp + ps * step;
                                    double pr = dr + rs * step;
                                    transform_360(t360support, proposalFrame, currentFrame, reducedWidth, reducedHeight, 0, reducedHeight,
                                        py, pp, pr,
                                        Interpolation::MONO_BILINEAR);
                                    int64_t score = diff(previousFrame, proposalFrame, reducedWidth, reducedHeight, best);
                                    if (score < best) {
                                        best = score;
                                        bys = ys;
                                        bps = ps;
                                        brs = rs;
                                    }
                                }
                            }
                        }
                    }
                    dy += bys * step;
                    dp += bps * step;
                    dr += brs * step;

                    step *= 0.5;
                }

                view (dy, dp, dr);

                rawSamples.add (Rotation(previousFrameTime + ROTATION_TIME_INSTANT, clipTime, dy, dp, dr, true));
            } else {
                view (0, 0, 0);
            }

            Graphics postXform (out, width, height);

            unsigned int diagramWidth = 512;
            if (diagramWidth > width / 2) {
                diagramWidth = width / 2;
            }
            unsigned int diagramHeight = 128;
            if (diagramHeight > height / 4) {
                diagramHeight = diagramHeight;
            }
            rawSamples.drawDiagram (postXform, clipTime, width / 2, 3 * height / 4, diagramWidth, diagramHeight);

            char buf[1024];
            size_t frames = rawSamples.size();
            double earliest = rawSamples.size() > 0 ? rawSamples[0].time : -1;
            double latest = rawSamples.size() > 0 ? rawSamples[rawSamples.size() - 1].time : -1;
            int skip = rawSamples.findFirstSkip();
            char skipBuf[256];
            if (skip < 0) {
                snprintf (skipBuf, 256, "No frame skips detected.");
            } else {
                snprintf (skipBuf, 256, "Frame skipped at %.2f s.", rawSamples[skip].time);
            }
            snprintf (buf, 1024,
                      "%s\n"
                      "At %.3fs -> %.3fs (%.2fs) %zd frames, from %.2f s to %.2f s\n"
                      "%s\n"
                      "Last frame motion: (%.3f, %.3f, %.3f)", analysisFile.c_str(),
                      previousFrameTime, clipTime, time,
                      frames, earliest, latest, skipBuf,
                      yaw, pitch, roll);
            std::string status(buf);

            postXform.drawText(8, 8, status, 0, 0xff0000ff);

            previousFrameTime = clipTime;
            if (previousFrame == NULL) {
                previousFrame = (uint32_t*) malloc(reducedWidth * reducedHeight * sizeof(uint32_t));
            }
            memcpy(previousFrame, currentFrame, reducedWidth * reducedHeight * sizeof(uint32_t));
        } else {
            if (smoothYaw.changed() || smoothPitch.changed() || smoothRoll.changed() ||
                    timeBiasYaw.changed() || timeBiasPitch.changed() || timeBiasRoll.changed()) {
                updateCorrections();
            }

            int correctionIndex = corrections.lookup (clipTime);
            if (correctionIndex >= 0) {
                Rotation correction = corrections[correctionIndex];
                view (
                    correction.yaw * stabilizeYaw / 100.0,
                    correction.pitch * stabilizePitch / 100.0,
                    correction.roll * stabilizeRoll / 100.0
                );

                MPFilter::updateMP(this, time, out, in, width, height);
            } else {
                view(0, 0, 0);
                MPFilter::updateMP(this, time, out, in, width, height);
            }

            previousFrameTime = -1;
            if (previousFrame != NULL) {
                free(previousFrame);
                previousFrame = NULL;
            }
        }
    }

    void view (double y, double p, double r) {
        yaw = y;
        pitch = p;
        roll = r;
    }

    virtual void updateLines(double time,
                             uint32_t* out,
                             const uint32_t* in, int start, int num) {
        transform_360(t360, out, (uint32_t*) in, width, height, start, num, yaw, pitch, roll, interpolation);
    }
};

frei0r::construct<Stabilize360v2> plugin("bigsh0t_stabilize_360_v2",
                                         "Stabilizes 360 equirectangular footage.",
                                         "Leo Sutic <leo@sutic.nu>",
                                         BIGSH0T_VERSION_MAJOR, BIGSH0T_VERSION_MINOR, F0R_COLOR_MODEL_PACKED32);
