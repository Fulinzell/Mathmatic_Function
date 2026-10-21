#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "Mathmatic_DataType.h"

// 计算三阶行列式的函数
double determinant(const Matrix3D &Matrix);

double determinant(const Matrix3DVec &matrix);

// 输出矩阵
void printMatrix(const Matrix3DVec &matrix);

#endif // __MATRIX_H__