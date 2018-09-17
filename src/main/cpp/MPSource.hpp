/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPSource_HPP
#define MPSource_HPP

#include <inttypes.h>

class MPSource {

public:
    MPSource() {
    }

    ~MPSource() {
    }
    
    virtual void updateLines(double time,
	                    uint32_t* out,
                        int start, int num) = 0;
                        
    static void updateMP(MPSource* filter, double time,
	                    uint32_t* out,
                        int width, int height);
};

#endif