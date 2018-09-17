#ifndef MPFilter_HPP
#define MPFilter_HPP

#include <inttypes.h>

class MPFilter {

public:
    MPFilter() {
    }

    ~MPFilter() {
    }
    
    virtual void updateLines(double time,
	                    uint32_t* out,
                        const uint32_t* in, int start, int num) = 0;
                        
    static void updateMP(MPFilter* filter, double time,
	                    uint32_t* out,
                        const uint32_t* in, int width, int height);
};

#endif