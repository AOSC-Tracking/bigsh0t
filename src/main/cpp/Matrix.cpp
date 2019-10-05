/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "Matrix.hpp"
#include <climits>
#include <cmath>

void rotateX(Matrix3& m, double a) {
    double ca = cos(a);
    double sa = sin(a);
    Matrix3 rm;
    rm[0] = 1.0; rm[1] = 0.0; rm[2] = 0.0;
    rm[3] = 0.0; rm[4] = ca;  rm[5] = -sa;
    rm[6] = 0.0; rm[7] = sa;  rm[8] = ca;
    m.prepend(rm);
}

void rotateY(Matrix3& m, double a) {
    double ca = cos(a);
    double sa = sin(a);
    Matrix3 rm;
    rm[0] = ca;  rm[1] = 0.0; rm[2] = sa;
    rm[3] = 0.0; rm[4] = 1.0; rm[5] = 0.0;
    rm[6] = -sa; rm[7] = 0.0; rm[8] = ca;
    m.prepend(rm);
}

void rotateZ(Matrix3& m, double a) {
    double ca = cos(a);
    double sa = sin(a);
    Matrix3 rm;
    rm[0] = ca;  rm[1] = -sa; rm[2] = 0.0;
    rm[3] = sa;  rm[4] =  ca; rm[5] = 0.0;
    rm[6] = 0.0; rm[7] = 0.0; rm[8] = 1.0;
    m.prepend(rm);
}

void mulM3V3(Matrix3& m, Vector3& v, Vector3& out) {
    out[0] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2];
    out[1] = m[3] * v[0] + m[4] * v[1] + m[5] * v[2];
    out[2] = m[6] * v[0] + m[7] * v[1] + m[8] * v[2];
}

void mulV3S(Vector3& v, double s, Vector3& out) {
    out[0] = s * v[0];
	out[1] = s * v[1];
	out[2] = s * v[2];
}

void addV3V3(Vector3& u, Vector3& v, Vector3& out) {
    out[0] = u[0] + v[0];
	out[1] = u[1] + v[1];
	out[2] = u[2] + v[2];
}
