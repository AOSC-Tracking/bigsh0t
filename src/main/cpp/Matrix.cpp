/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <climits>
#include <cmath>

#include "Matrix.hpp"

void rotateX(Matrix3& m, double a) {
    double ca = cos(a);
    double sa = sin(a);
    Matrix3 rm;
    rm[0] = 1.0;
    rm[1] = 0.0;
    rm[2] = 0.0;
    rm[3] = 0.0;
    rm[4] = ca;
    rm[5] = -sa;
    rm[6] = 0.0;
    rm[7] = sa;
    rm[8] = ca;
    m.prepend(rm);
}

void rotateY(Matrix3& m, double a) {
    double ca = cos(a);
    double sa = sin(a);
    Matrix3 rm;
    rm[0] = ca;
    rm[1] = 0.0;
    rm[2] = sa;
    rm[3] = 0.0;
    rm[4] = 1.0;
    rm[5] = 0.0;
    rm[6] = -sa;
    rm[7] = 0.0;
    rm[8] = ca;
    m.prepend(rm);
}

void rotateZ(Matrix3& m, double a) {
    double ca = cos(a);
    double sa = sin(a);
    Matrix3 rm;
    rm[0] = ca;
    rm[1] = -sa;
    rm[2] = 0.0;
    rm[3] = sa;
    rm[4] =  ca;
    rm[5] = 0.0;
    rm[6] = 0.0;
    rm[7] = 0.0;
    rm[8] = 1.0;
    m.prepend(rm);
}

void mulM3V3(const Matrix3& m, const Vector3& v, Vector3& out) {
    out[0] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2];
    out[1] = m[3] * v[0] + m[4] * v[1] + m[5] * v[2];
    out[2] = m[6] * v[0] + m[7] * v[1] + m[8] * v[2];
}

void mulV3S(const Vector3& v, const double s, Vector3& out) {
    out[0] = s * v[0];
    out[1] = s * v[1];
    out[2] = s * v[2];
}

void addV3V3(const Vector3& u, const Vector3& v, Vector3& out) {
    out[0] = u[0] + v[0];
    out[1] = u[1] + v[1];
    out[2] = u[2] + v[2];
}

void crossV3V3(const Vector3& u, const Vector3& v, Vector3& out) {
    out[0] = u[1]*v[2] - u[2]*v[1];
    out[1] = u[2]*v[0] - u[0]*v[2];
    out[2] = u[0]*v[1] - u[1]*v[0];
}

double dotV3V3(const Vector3& u, const Vector3& v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

void mulQQ(const Quaternion& q1, const Quaternion& q2, Quaternion& out) {
    out[0] = -q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3] + q1[0] * q2[0];
    out[1] =  q1[1] * q2[0] + q1[2] * q2[3] - q1[3] * q2[2] + q1[0] * q2[1];
    out[2] = -q1[1] * q2[3] + q1[2] * q2[0] + q1[3] * q2[1] + q1[0] * q2[2];
    out[3] =  q1[1] * q2[2] - q1[2] * q2[1] + q1[3] * q2[0] + q1[0] * q2[3];
}

void invertQ(const Quaternion& v, Quaternion& out) {
    out[0] = v[0];
    out[1] = -v[1];
    out[2] = -v[2];
    out[3] = -v[3];
}

void rotateQuaternion(Matrix3& m, const Quaternion& q) {
    double s = q.norm();
    double r = q[0];
    double i = q[1];
    double j = q[2];
    double k = q[3];

    Matrix3 rm;
    rm[0] = 1 - 2*s*(j*j + k*k);
    rm[1] =     2*s*(i*j - k*r);
    rm[2] =     2*s*(i*k + j*r);
    rm[3] =     2*s*(i*j + k*r);
    rm[4] = 1 - 2*s*(i*i + k*k);
    rm[5] =     2*s*(j*k - i*r);
    rm[6] =     2*s*(i*k - j*r);
    rm[7] =     2*s*(j*k + i*r);
    rm[8] = 1 - 2*s*(i*i + j*j);
    m.prepend(rm);
}

/**
 * Decomposes a quaternion rotation into swing (rotation around a vector perpendicular to v),
 * and twist (rotation around v)
 */
void decomposeQ(const Quaternion& q, const Vector3& v, Quaternion& swing, Quaternion& twist) {
    Vector3 ra;
    ra[0] = q[1];
    ra[1] = q[2];
    ra[2] = q[3];
    Vector3 p;
    //p = projection(ra, v); // return projection v1 on to v2  (parallel component)
    Vector3 vNorm(v);
    vNorm.normalize();

    double scalarP = ra[0] * vNorm[0] + ra[1] * vNorm[1] + ra[2] * vNorm[2];
    mulV3S(vNorm, scalarP, p);

    twist[0] = q[0];
    twist[1] = p[0];
    twist[2] = p[1];
    twist[3] = p[2];
    twist.normalize();
    Quaternion twistConj;
    invertQ(twist, twistConj);
    mulQQ(q, twistConj, swing);
}
