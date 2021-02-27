/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef Frei0rFilter_HPP
#define Frei0rFilter_HPP

#include "Frei0rParameter.hpp"

class Frei0rFilter : public frei0r::filter {

  public:
    Frei0rFilter(unsigned int width, unsigned int height) {
    }

    ~Frei0rFilter() {
    }

    void register_fparam(Frei0rParameter<int,double>& param, const char* name, const char* desc) {
        register_param(param.parameter, name, desc);
    }

    void register_fparam(Frei0rParameter<double,double>& param, const char* name, const char* desc) {
        register_param(param.parameter, name, desc);
    }
};

#endif
