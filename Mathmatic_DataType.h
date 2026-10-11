#ifndef __MATHMATIC_DATATYPE_H__
#define __MATHMATIC_DATATYPE_H__

#include <vector> // 引入vector头文件

////////////////////////////////////////POINT_2D/////////////////////////////////////////////
// 二维点坐标
typedef struct Point_2D
{
    double X_Pos;
    double Y_Pos;
    double K_Val;
} POINT_2D;

// 三维点坐标
typedef struct Point_3D
{
    double X_Pos;
    double Y_Pos;
    double Z_Pos;
    double K_Val; // 斜率值
} POINT_3D;

/////////////////////////////////////////LINE_2D/////////////////////////////////////////////
// 定义二阶矩阵结构
typedef struct Matrix2D
{
    double matrixelem[2][2];
} MATRIX2D;

//////////////////////////////////////////MATRIX3D////////////////////////////////////////////
// 定义三阶矩阵结构
typedef struct Matrix3D
{
    double matrixelem[3][3];
} MATRIX3D;

// 定义一个3x3矩阵
typedef std::vector<std::vector<double>> MATRIX_3D;

#endif