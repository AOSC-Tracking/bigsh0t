/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <math.h>
#include <iostream>

template<unsigned int R, unsigned int C>
class Matrix {
  public:
    Matrix() {
    }

    Matrix(const Matrix& other) {
        for (int i = 0; i < rows * cols; ++i) {
            elements[i] = other.elements[i];
        }
    }

    void zero() {
        for (int i = 0; i < rows * cols; ++i) {
            elements[i] = 0;
        }
    }

    void identity() {
        zero();
        for (int i = 0; i < rows; ++i) {
            elements[i * C + i] = 1;
        }
    }

    double& operator[](int index) {
        return elements[index];
    }

    double const& operator[](int index) const {
        return elements[index];
    }

    double norm2() const {
        double sum = 0;
        for (int i = 0; i < rows * cols; ++i) {
            double el = elements[i];
            sum += el * el;
        }
        return sum;
    }

    double norm() const {
        return sqrt(norm2());
    }

    bool normalize() {
        double n = norm();
        if (n < 0.000001) {
            return false;
        }
        double factor = 1.0 / n;
        for (int i = 0; i < rows * cols; ++i) {
            elements[i] *= factor;
        }
        return true;
    }

    void prepend(Matrix<R,C>& other) {
        Matrix<R,C> res;
        res.zero();
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                for (int k = 0; k < cols; ++k) {
                    res[i * cols + j] += other[i * cols + k] * elements[k * cols + j];
                }
            }
        }
        for (int i = 0; i < rows * cols; ++i) {
            elements[i] = res[i];
        }
    }

    void setQuaternionRotation(double a, double x, double y, double z) {
        double s = sin(a / 2);
        double c = cos(a / 2);
        elements[0] = c;
        elements[1] = s * x;
        elements[2] = s * y;
        elements[3] = s * z;
    }

  private:
    const int rows = R;
    const int cols = C;
    double elements[R * C];
};

template<unsigned int R, unsigned int C>
std::ostream &operator<<(std::ostream &os, Matrix<R,C> const &m) {
    os << "{";
    for (int row = 0; row < R; ++row) {
        os << "[";
        for (int col = 0; col < C; ++col) {
            if (col > 0) {
                os << " ";
            }
            os << m[row * C + col];
        }
        os << "]";
    }
    os << "}";
    return os;
}

typedef Matrix<3,3> Matrix3;
typedef Matrix<1,3> Vector3;
typedef Matrix<1,2> Vector2;
typedef Matrix<1,4> Quaternion;

void rotateX(Matrix3& m, double a);
void rotateY(Matrix3& m, double a);
void rotateZ(Matrix3& m, double a);
void rotateQuaternion(Matrix3& m, const Quaternion& q);

void rotateQV3(const Quaternion& q, const Vector3& v, Vector3& out);
void mulM3V3(const Matrix3& m, const Vector3& v, Vector3& out);
void mulV3S(const Vector3& v, const double s, Vector3& out);
void addV3V3(const Vector3& u, const Vector3& v, Vector3& out);
void crossV3V3(const Vector3& u, const Vector3& v, Vector3& out);
double dotV3V3(const Vector3& u, const Vector3& v);
void mulQQ(const Quaternion& q1, const Quaternion& q2, Quaternion& out);
void invertQ(const Quaternion& v, Quaternion& out);
void decomposeQ(const Quaternion& q, const Vector3& v, Quaternion& swing, Quaternion& twist);

#endif
