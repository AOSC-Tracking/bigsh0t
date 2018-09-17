#ifndef MATRIX_HPP
#define MATRIX_HPP

template<unsigned int R, unsigned int C>
class Matrix {
public:
    Matrix() {
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
    
private:
    const int rows = R;
    const int cols = C;
    double elements[R * C];
};

typedef Matrix<3,3> Matrix3;
typedef Matrix<1,3> Vector3;

void rotateX(Matrix3& m, double a);
void rotateY(Matrix3& m, double a);
void rotateZ(Matrix3& m, double a);

void mulM3V3(Matrix3& m, Vector3& v, Vector3& out);

#endif
