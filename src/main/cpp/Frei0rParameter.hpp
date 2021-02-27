/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef Frei0rParameter_HPP
#define Frei0rParameter_HPP

template <typename T, typename U>
class Frei0rParameter {
  public:
    U parameter;
    T value;

    Frei0rParameter() {
    }

    bool changed () {
        return value != ((T) parameter);
    }

    T read () {
        value = (T) parameter;
        return value;
    }

    T get () {
        return value;
    }

    operator T() {
        return read();
    }

    Frei0rParameter& operator = (const T& v) {
        value = v;
        parameter = (U) v;
        return *this;
    }
};

#endif
