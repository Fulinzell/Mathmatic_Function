#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "Mathmatic_DataType.h"

// 计算三阶行列式的函数
double calculate_Determinant(const MATRIX3D &Matrix);

double calculate_Determinant(const MATRIX_3D &matrix);

// 输出矩阵
void print_Matrix(const MATRIX_3D &matrix);

#endif // __MATRIX_H__