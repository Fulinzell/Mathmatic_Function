#include "Head.h"
#include <string>

// 计算三阶行列式的函数
/*
    输入：@param Matrix 三阶矩阵
    输出：返回矩阵的行列式值
    计算公式：det(A) = a(ei - fh) - b(di - fg) + c(dh - eg)
    其中，a, b, c, d, e, f, g, h, i 分别是矩阵的元素。
*/
double determinant(const Matrix3D& Matrix) 
{
    double a = Matrix.matrixElem[0][0];
    double b = Matrix.matrixElem[0][1];
    double c = Matrix.matrixElem[0][2];
    double d = Matrix.matrixElem[1][0];
    double e = Matrix.matrixElem[1][1];
    double f = Matrix.matrixElem[1][2];
    double g = Matrix.matrixElem[2][0];
    double h = Matrix.matrixElem[2][1];
    double i = Matrix.matrixElem[2][2];

    double det = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
    return det;
}

// 计算3x3矩阵的行列式
/*    输入：@param matrix 三阶矩阵
      输出：返回矩阵的行列式值
      计算公式：det(A) = a(ei - fh) - b(di - fg) + c(dh - eg)
      其中，a, b, c, d, e, f, g, h, i 分别是矩阵的元素。
*/
double determinant(const Matrix3DVec& matrix) 
{
    return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
           matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
           matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
}


// 输出矩阵
/*
    输入：@param matrix 三阶矩阵
    输出：打印矩阵到标准输出
    功能：格式化输出矩阵，使其易于阅读
    注意：矩阵元素的宽度会根据最大元素长度自动调整
*/
void printMatrix(const vector<vector<double>>& matrix) 
{
    if (matrix.empty()) return;

    size_t rows = matrix.size();
    size_t cols = matrix[0].size();

    vector<int> colWidths(cols, 0);
    for (size_t i = 0; i < rows; ++i) 
    {
        for (size_t j = 0; j < cols; ++j) 
        {
            colWidths[j] = max(colWidths[j], static_cast<int>(to_string(matrix[i][j]).length()));
        }
    }

    for (size_t i = 0; i < rows; ++i) 
    {
        if (i == 0) 
        {
            cout << "[";
        } 
        else 
        {
            cout << "     ";
        }
        for (size_t j = 0; j < cols; ++j) 
        {
            cout << setw(colWidths[j]) << matrix[i][j];
            if (j < cols - 1) cout << ", ";
        }
        if (i == rows - 1) 
        {
            cout << "]";
        }
        cout << endl;
    }
}