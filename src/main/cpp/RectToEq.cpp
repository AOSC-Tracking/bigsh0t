#include "frei0r.hpp"
#include "Matrix.hpp"
#include "MPFilter.hpp"
#include "ImageProcessing.hpp"
#include <limits>
#include <climits>
#include <cmath>

#define M_PI           3.14159265358979323846  
#define DEG2RADF(x) ((x) * M_PI / 180.0)

class RectToEq : public frei0r::filter, MPFilter {

public:
    double hfov;
    double vfov;
    double interpolationParam;
    int interpolation;
	
    
    RectToEq(unsigned int width, unsigned int height) {
        register_param(hfov, "hfov", "");
        register_param(vfov, "vfov", "");
        register_param(interpolationParam, "interpolation", "");
        
        hfov = 90;
        vfov = 60;

        interpolation = Interpolation::BILINEAR;
    }

    ~RectToEq() {
    }
    
    virtual void update(double time,
	                    uint32_t* out,
                        const uint32_t* in) {
        interpolation = interpolationParam;
        MPFilter::updateMP(this, time, out, in, width, height);
    }
    
    virtual void updateLines(double time,
	                    uint32_t* out,
                        const uint32_t* in, int start, int num) {
        rect_to_eq_thread(out, (uint32_t*) in, start, num);     
    }

protected:
    void rect_to_eq_thread(uint32_t* out, uint32_t* ibuf1, int start_scanline, int num_scanlines) {
        
        int w = width;
        int h = height;

        float hfovR = DEG2RADF(hfov);
        float vfovR = DEG2RADF(vfov);

        int xi, yi;
        float xt, yt;

        float x0 = -tan(hfovR / 2);
        float y0 = -tan(vfovR / 2);
        float xs = -2 * x0;
        float ys = -2 * y0;

        Vector3 ray;

        int x_width = hfovR * w / (2 * M_PI);

        int min_x = w / 2 - x_width / 2 - 1;
        int max_x = w / 2 + x_width / 2 + 1;
        if (min_x < 0) {
            min_x = 0;
        }
        if (max_x > w - 1) {
            max_x = w - 1;
        }
        
        memset(&out[start_scanline * width], 0, num_scanlines * width * 4);

        for (yi = start_scanline; yi < start_scanline + num_scanlines; yi++) {
            float phi = M_PI * ((float) yi - h / 2) / h;
            float cos_phi = cos(phi);
            float sin_phi = sin(phi);
            for (xi = min_x; xi < max_x; xi++) {
                float theta = 2 * M_PI * ((float) xi - w / 2) / w;

                ray[0] = cos(theta) * cos_phi;

                if (ray[0] > 0) {
                    ray[1] = sin(theta) * cos_phi;
                    ray[2] = sin_phi;

                    ray[1] /= ray[0];
                    ray[2] /= ray[0];

                    xt = w * (ray[1] - x0) / xs;
                    yt = h * (ray[2] - y0) / ys;

                    if (xt >= 0 && yt >= 0 && xt < w - 1 && yt < h - 1) {
                        uint32_t pixel;
                        switch(interpolation) {
                            case Interpolation::NONE:
                                pixel = sampleNearestNeighbor(ibuf1, xt, yt, width, height);
                                break;
                            case Interpolation::BILINEAR:
                                pixel = sampleBilinear(ibuf1, xt, yt, width, height);
                                break;
                        }
                        out[((int) yi) * width + ((int) xi)] = pixel;
                    }
                }
            }
        }
    }

    
private:
    
};

frei0r::construct<RectToEq> plugin("rect_to_eq",
                "Converts a rectilinear image to an equirectangular map.",
                "Leo Sutic <leo@sutic.nu>",
                1, 0, F0R_COLOR_MODEL_PACKED32);