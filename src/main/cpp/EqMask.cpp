#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include <limits>
#include <climits>
#include <cmath>

#define M_PI           3.14159265358979323846  
#define DEG2RADF(x) ((x) * M_PI / 180.0)


class EqMask : public frei0r::filter, MPFilter {

public:
    double hfov0;
	double hfov1;
    double vfov0;
    double vfov1;
    int interpolation;
	    
    EqMask(unsigned int width, unsigned int height) {
        register_param(hfov0, "hfov0", "");
        register_param(hfov1, "hfov1", "");
        register_param(vfov0, "vfov0", "");
        register_param(vfov1, "vfov1", "");
        
        hfov0 = 160;
        hfov1 = 180;
        vfov0 = 120;
        vfov1 = 140;

        interpolation = 0;
    }

    ~EqMask() {
    }
    
    virtual void update(double time,
	                    uint32_t* out,
                        const uint32_t* in) {
        MPFilter::updateMP(this, time, out, in, width, height);
    }
    
    virtual void updateLines(double time,
	                    uint32_t* out,
                        const uint32_t* in,
                        int start, int num) {
                            
        double coshfov0 = cos(DEG2RADF(hfov0) / 2);
        double coshfov1 = cos(DEG2RADF(hfov1) / 2);
        double coshfovd = coshfov0 - coshfov1;
        
        double sinvfov0 = sin(DEG2RADF(vfov0) / 2);
        double sinvfov1 = sin(DEG2RADF(vfov1) / 2);
        double sinvfovd = sinvfov0 - sinvfov1;
        
        for (int y = start; y < (start + num); ++y) {
            double phi = (-M_PI / 2) + (y * M_PI / height);
            double cosPhi = cos(phi);
            double sinPhi = sin(phi);
            
            double ry = sinPhi;
            
            double vv = 1.0;
            if (ry > sinvfov1) {
                vv = 0.0;
            } else if (ry > sinvfov0) {
                vv = (ry - sinvfov1) / sinvfovd;
            }
            vv = smooth (vv);
            
            for (int x = 0; x < width; ++x) {
                double theta = M_PI - (2 * M_PI * x / width);
                double cosTheta = cos(theta);
                
                double z = cosTheta * cosPhi;
                
                double vh = 1.0;
                if (z < coshfov1) {
                    vh = 0.0;
                } else if (z < coshfov0) {
                    vh = (z - coshfov1) / coshfovd;
                }
                
                vh = smooth (vh);
                
                double v = vh * vv;
                int vInt = v * 256;
                if (vInt > 255) {
                    vInt = 255;
                }
                if (vInt < 0) {
                    vInt = 0;
                }
                int offset = y * width + x;
                unsigned char* inP = (unsigned char*) (in + offset);
                unsigned char* outP = (unsigned char*) (out + offset);
                for (int c = 0; c < 3; ++c) {
                    outP[c] = inP[c] * vInt >> 8;
                }
            }
        }
    }

protected:
    double smooth (double v) {
        double v2 = 1.0 - (1.0 - v) * (1.0 - v);
        double blend = 1.0 - v;
        return v * blend + (1.0 - blend) * v2;
    }
    
private:
    
};

frei0r::construct<EqMask> plugin("eq_mask",
                "Masks part of the VR sphere.",
                "Leo Sutic <leo@sutic.nu>",
                1, 0, F0R_COLOR_MODEL_RGBA8888);