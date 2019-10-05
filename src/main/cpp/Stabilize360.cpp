/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <limits>
#include <climits>
#include <cmath>
#include <vector>
#include <string>
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

#define ROTATION_TIME_INSTANT (1.0 / 10000.0)


class Rotation {

	public:
	double previousTime;
	double time;
	double yaw;
	double pitch;
	double roll;

	Rotation (double previousTime, double time, double yaw, double pitch, double roll) {
		this->previousTime = previousTime;
		this->time = time;
		this->yaw = yaw;
		this->pitch = pitch;
		this->roll = roll;
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
	}
};

class RotationSamples {
	public:
	std::vector<Rotation> rotations;

	void add (Rotation rot) {
		removeOverlapping (rot);
		if (rotations.size() == 0) {
			rotations.push_back(rot);
			return;
		}
		size_t pos = 0;
		for (size_t i = rotations.size() - 1; i >= 0; --i) {
			if (rotations[i].time < rot.time) {
				pos = i + 1;
				break;
			}
		}
		if (pos == rotations.size()) {
			rotations.push_back(rot);
		} else {
			rotations.insert (rotations.begin() + pos, rot);
		}
	}

	Rotation getMax () {
		Rotation m (0, 0, 0, 0, 0);
		for (Rotation& r : rotations) {
			double ay = abs(r.yaw);
			double ap = abs(r.pitch);
			double ar = abs(r.roll);
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
				Rotation r(0, 0, 0, 0, 0);
				r.readFrom (file);
				rotations.push_back (r);
			}
			file.close();
		}
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
	}
	
	void correct (int wYaw, int wPitch, int wRoll, double bYaw, double bPitch, double bRoll, RotationSamples& dest) {
		std::vector<Rotation> accs;
		{
			Rotation acc (0, 0, 0, 0, 0);
			for (Rotation& r : rotations) {
				acc.yaw += r.yaw;
				acc.pitch += r.pitch;
				acc.roll += r.roll;

				Rotation r2 (r.previousTime, r.time, acc.yaw, acc.pitch, acc.roll);
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
				
				Rotation smooth (r.previousTime, r.time, cYaw[i], cPitch[i], cRoll[i]);

				smooths.push_back (smooth);
			}
		}

		for (int i = 0; i < accs.size(); ++i) {
			Rotation& measured = accs[i];
			Rotation& smoothed = smooths[i];

			Rotation correction (measured.previousTime, measured.time, smoothed.yaw - measured.yaw, smoothed.pitch - measured.pitch, smoothed.roll - measured.roll);
			dest.add(correction);
		}
	}
};

inline short toGray(uint32_t color) {
	return
	((color      ) & 0xff) +
	((color >>  8) & 0xff) +
	((color >> 16) & 0xff);
}

class TrackPoint {
	public:

	TrackPoint (int x, int y, int sampleRadius, int searchRadius) {
		this->x = x;
		this->y = y;
		this->cx = x;
		this->cy = y;
		this->sampleRadius = sampleRadius;
		this->searchRadius = searchRadius;

		sampleBuffer = NULL;
		active = true;
	}

	~TrackPoint() {
		if (sampleBuffer != NULL) {
			free(sampleBuffer);
		}
	}

	int match (Graphics& g, const uint32_t* buffer, int atx, int aty, int abortAtError) {
		int error = 0;
		int sbp = 0;
		for (int sy = aty; sy < aty + sampleRadius * 2; ++sy) {
			for (int sx = atx; sx < atx + sampleRadius * 2; ++sx) {
				int sample = sampleBuffer[sbp];
				int actual = toGray(buffer[sy * g.width + sx]);
				int err = abs(sample - actual);
				error += err;
				++sbp;
				if (error > abortAtError) {
					return error;
				}
			}
		}
		return error;
	}

	void update (Graphics& g, const uint32_t* previous, const uint32_t* current) {
		active = true;

		if (sampleBuffer == NULL) {
			sampleBuffer = (short*) malloc(sampleRadius * sampleRadius * 4 * sizeof(short));
		}

		int sbp = 0;
		for (int sy = y - sampleRadius; sy < y + sampleRadius; ++sy) {
			for (int sx = x - sampleRadius; sx < x + sampleRadius; ++sx) {
				sampleBuffer[sbp] = toGray(previous[sy * g.width + sx]);
				++sbp;
			}
		}

		cx = x;
		cy = y;

		int bestError = sampleRadius * sampleRadius * 4 * 256 * 3;
		for (int my = y - searchRadius; my < y + searchRadius; ++my) {
			for (int mx = x - searchRadius; mx < x + searchRadius; ++mx) {
				int error = match (g, current, mx - sampleRadius, my - sampleRadius, bestError);
				if (bestError < 0 || error < bestError) {
					bestError = error;
					cx = mx;
					cy = my;
					cerr = bestError;
				}
			}
		}
	}

	void markOrigin (Graphics& g) {
		g.drawRect(x - sampleRadius, y - sampleRadius, 2 * sampleRadius, 2 * sampleRadius, 0xffffff00, 0xff0000ff);
		g.drawRect(x - sampleRadius + 1, y - sampleRadius + 1, 2 * sampleRadius - 2, 2 * sampleRadius - 2, 0xffffff00, 0xff0000ff);
		g.drawRect(x - sampleRadius - searchRadius, y - sampleRadius - searchRadius, 
			2 * searchRadius + 2 * sampleRadius, 2 * searchRadius + 2 * sampleRadius, 0xffff0000, 0xff00ffff);
	}

	void markOriginTransformed (Graphics& g) {
		g.fillRect(x - sampleRadius, y - sampleRadius, 2 * sampleRadius, 2 * sampleRadius, 0xff00ffff, 0xffff0000);
	}

	void markCurrent (Graphics& g) {
		if (active) {
			g.fillRect(cx - sampleRadius, cy - sampleRadius, 2 * sampleRadius, 2 * sampleRadius, 0xffff00ff, 0xff00ff00);
		}
	}

	void getMotion (Vector2& motion) {
		motion[0] = cx - x;
		motion[1] = cy - y;
	}

	int getError () {
		return cerr;
	}

	void setActive (bool active) {
		this->active = active;
	}

	private:
	int x;
	int y;
	int cx;
	int cy;
	int cerr;
	int sampleRadius;
	int searchRadius;
	short* sampleBuffer;
	bool active;
};

class TrackPointMatrix {
	public:
	TrackPointMatrix (int x, int y, int numh, int numv, int offset, int sampleRadius, int searchRadius) {
		this->x = x;
		this->y = y;
		this->sampleRadius = sampleRadius;
		this->searchRadius = searchRadius;

		int x0 = x - (offset * numh / 2) + offset / 2;
		int y0 = y - (offset * numv / 2) + offset / 2;
		for (int tpy = 0; tpy < numv; ++tpy) {
			for (int tpx = 0; tpx < numh; ++tpx) {
				trackPoints.push_back (TrackPoint (x0 + tpx * offset, y0 + tpy * offset, sampleRadius, searchRadius));
				errors.push_back (0);
			}
		}
	}

	~TrackPointMatrix() {
	}

	void update (Graphics& g, const uint32_t* previous, const uint32_t* current) {
		#pragma omp parallel for
		for (int i = 0; i < trackPoints.size(); ++i) {
			TrackPoint& tp = trackPoints[i];
			tp.update (g, previous, current);
		}
	}

	void markOrigin (Graphics& g) {
		for (TrackPoint& tp : trackPoints) {
			tp.markOrigin (g);
		}
	}

	void markOriginTransformed (Graphics& g) {
		for (TrackPoint& tp : trackPoints) {
			tp.markOriginTransformed (g);
		}
	}

	void markCurrent (Graphics& g) {
		for (TrackPoint& tp : trackPoints) {
			tp.markCurrent (g);
		}
	}

	void getMotion (Vector2& motion) {
		Vector2 acc;
		acc.zero ();
		Vector2 tpm;
		int totalWeights = 0;
		int maxError = sampleRadius * sampleRadius * 3 * 255;
		int avgError = 0;
		for (int i = 0; i < trackPoints.size(); ++i) {
			int err = trackPoints[i].getError();
			errors[i] = err;
			avgError += err;
		}
		avgError /= (int) trackPoints.size();
		std::sort(errors.begin(), errors.end());
		int maxAllowedError = errors[errors.size() * 2 / 3];
		for (TrackPoint& tp : trackPoints) {
			tp.getMotion (tpm);
			if (tp.getError() <= maxAllowedError) {
				int weight = maxError - tp.getError ();
				acc[0] += tpm[0] * weight;
				acc[1] += tpm[1] * weight;
				totalWeights += weight;
				tp.setActive (true);
			} else {
				tp.setActive (false);
			}
		}
		acc[0] /= totalWeights;
		acc[1] /= totalWeights;
		motion[0] = acc[0];
		motion[1] = acc[1];
	}

	private:
	int x;
	int y;
	int sampleRadius;
	int searchRadius;
	std::vector<TrackPoint> trackPoints;
	std::vector<int> errors;
};

class Stabilize360 : public Frei0rFilter, MPFilter {

	private:
	bool previousAnalyzeState;
	bool initializedAnalyzeState;

	double yaw;
	double pitch;
	double roll;

	RotationSamples rawSamples;
	RotationSamples corrections;

	std::mutex lock;

	public:
	Frei0rParameter<int,double> interpolation;
	bool analyze;
	bool useBackTrackpoints;

	Frei0rParameter<int,double> sampleRadius;
	Frei0rParameter<int,double> searchRadius;
	Frei0rParameter<int,double> offset;

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

	uint32_t* previousFrame;
	double previousFrameTime;


	Stabilize360(unsigned int width, unsigned int height) : Frei0rFilter(width, height) {
		initializedAnalyzeState = false;
		previousAnalyzeState = false;

		interpolation = Interpolation::BILINEAR;

		sampleRadius = 16;
		searchRadius = 24;
		offset = 64;

		previousFrame = NULL;
		previousFrameTime = -1;
		analyze = false;

		useBackTrackpoints = false;

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
		register_fparam(interpolation, "interpolation", "");
		register_param(analyze, "analyze", "");
		register_fparam(sampleRadius, "sampleRadius", "");
		register_fparam(searchRadius, "searchRadius", "");
		register_fparam(offset, "offset", "");

		register_param(stabilizeYaw, "stabilizeYaw", "");
		register_param(stabilizePitch, "stabilizePitch", "");
		register_param(stabilizeRoll, "stabilizeRoll", "");

		register_fparam(smoothYaw, "smoothYaw", "");
		register_fparam(smoothPitch, "smoothPitch", "");
		register_fparam(smoothRoll, "smoothRoll", "");
		
		register_fparam(timeBiasYaw, "timeBiasYaw", "");
		register_fparam(timeBiasPitch, "timeBiasPitch", "");
		register_fparam(timeBiasRoll, "timeBiasRoll", "");

		register_param(useBackTrackpoints, "useBackTrackpoints", "");
	}

	virtual ~Stabilize360() {
		if (analyze) {
			endAnalyze ();
		} else {
			endApply ();
		}

		if (previousFrame != NULL) {
			free(previousFrame);
			previousFrame = NULL;
		}
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
			rawSamples.write (analysisFile);
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

	double weighted (double a, double aw, double b, double bw) {
		return (a * aw + b * bw) / (aw + bw);
	}

	double weighted (double a, double aw, double b, double bw, double c, double cw) {
		return (a * aw + b * bw + c * cw) / (aw + bw + cw);
	}

	virtual void update(double time, uint32_t* out, const uint32_t* in) {
		// frei0r filter instances are not thread-safe. Shotcut ignores that, so we'll
		// deal with it by wrapping the execution in a mutex
		std::lock_guard<std::mutex> guard(lock);

		updateAnalyzeState (time, out, in);

		if (analyze) {
			TrackPointMatrix trackAhead (        width / 2, height / 2, 3, 3, offset, sampleRadius, searchRadius);
			TrackPointMatrix trackLeft  (        width / 4, height / 2, 3, 3, offset, sampleRadius, searchRadius);
			TrackPointMatrix trackRight (width - width / 4, height / 2, 3, 3, offset, sampleRadius, searchRadius);
			TrackPointMatrix trackBackL (               64, height / 2, 1, 3, offset, sampleRadius, searchRadius);
			TrackPointMatrix trackBackR (       width - 64, height / 2, 1, 3, offset, sampleRadius, searchRadius);

			uint32_t* intermediateFrame = (uint32_t*) malloc(width * height * sizeof(uint32_t));
			memcpy (intermediateFrame, in, width * height * sizeof(uint32_t));

			Graphics preXform (intermediateFrame, width, height);

			bool updated = false;
			if (previousFrame != NULL && previousFrameTime < time) {
				trackAhead.update(preXform, previousFrame, in);
				trackLeft.update(preXform, previousFrame, in);
				trackRight.update(preXform, previousFrame, in);
				if (useBackTrackpoints) {
					trackBackL.update(preXform, previousFrame, in);
					trackBackR.update(preXform, previousFrame, in);
				}
				updated = true;

				double xScale = 360.0 / width;
				double yScale = 180.0 / height;
				Vector2 aheadMotion;
				trackAhead.getMotion (aheadMotion);

				Vector2 leftMotion;
				trackLeft.getMotion (leftMotion);

				Vector2 rightMotion;
				trackRight.getMotion (rightMotion);

				Vector2 backMotionL;
				trackBackL.getMotion (backMotionL);

				Vector2 backMotionR;
				trackBackR.getMotion (backMotionR);

				double backTrackpointWeight = useBackTrackpoints ? 0.3 : 0.0;

				double dYaw = -weighted(aheadMotion[0], 1.0, backMotionL[0], backTrackpointWeight, backMotionR[0], backTrackpointWeight) * xScale; // if the point has moved left, we have turned right (+yaw)
				double dPitch = weighted(aheadMotion[1], 1.0, -backMotionL[1], backTrackpointWeight, -backMotionR[1], backTrackpointWeight) * yScale; // if the point has moved down, we have pitched up (+pitch)
				double dRoll = weighted(leftMotion[1], 1.0, -rightMotion[1], 1.0) * yScale; // if the left point has moved down, we have rolled right (+roll)

				view (-dYaw, -dPitch, -dRoll);

				rawSamples.add (Rotation (previousFrameTime + ROTATION_TIME_INSTANT, time, dYaw, dPitch, dRoll));
			} else {
				view (0, 0, 0);
			}

			trackAhead.markOrigin(preXform);
			trackLeft.markOrigin(preXform);
			trackRight.markOrigin(preXform);
			if (useBackTrackpoints) {
				trackBackL.markOrigin(preXform);
				trackBackR.markOrigin(preXform);
			}

			trackAhead.markCurrent(preXform);
			trackLeft.markCurrent(preXform);
			trackRight.markCurrent(preXform);
			if (useBackTrackpoints) {
				trackBackL.markCurrent(preXform);
				trackBackR.markCurrent(preXform);
			}

			MPFilter::updateMP(this, time, out, intermediateFrame, width, height);

			Graphics postXform (out, width, height);
			trackAhead.markOriginTransformed(postXform);
			trackLeft.markOriginTransformed(postXform);
			trackRight.markOriginTransformed(postXform);
			if (useBackTrackpoints) {
				trackBackL.markOriginTransformed(postXform);
				trackBackR.markOriginTransformed(postXform);
			}

			unsigned int diagramWidth = 512;
			if (diagramWidth > width / 2) {
				diagramWidth = width / 2;
			}
			unsigned int diagramHeight = 128;
			if (diagramHeight > height / 4) {
				diagramHeight = diagramHeight;
			}
			rawSamples.drawDiagram (postXform, time, width / 2, 3 * height / 4, diagramWidth, diagramHeight);

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
			"At %.2f s. %zd frames, from %.2f s to %.2f s\n"
			"%s\n"
			"Last frame motion: (%.3f, %.3f, %.3f)", analysisFile.c_str(), 
			time, 
			frames, earliest, latest, skipBuf,
			yaw, pitch, roll);
			std::string status(buf);

			postXform.drawText(8, 8, status, 0, 0xff0000ff);

			free (intermediateFrame);

			previousFrameTime = time;
			if (previousFrame == NULL) {
				previousFrame = (uint32_t*) malloc(width * height * sizeof(uint32_t));
			}
			memcpy (previousFrame, in, width * height * sizeof(uint32_t));
		} else {
			if (smoothYaw.changed() || smoothPitch.changed() || smoothRoll.changed() ||
				timeBiasYaw.changed() || timeBiasPitch.changed() || timeBiasRoll.changed()) {
				updateCorrections();
			}
				
			int correctionIndex = corrections.indexOf (time);
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
		transform_360(out, (uint32_t*) in, width, height, start, num, yaw, pitch, roll, interpolation);
	}
};

frei0r::construct<Stabilize360> plugin("stabilize_360",
"Stabilizes 360 equirectangular footage.",
"Leo Sutic <leo@sutic.nu>",
2, 1, F0R_COLOR_MODEL_PACKED32);
