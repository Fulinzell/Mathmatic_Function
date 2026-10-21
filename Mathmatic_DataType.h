#ifndef __MATHMATIC_DATATYPE_H__
#define __MATHMATIC_DATATYPE_H__

#include <vector> // 引入vector头文件

////////////////////////////////////////Point2D/////////////////////////////////////////////
// 二维点坐标
typedef struct Point_2D
{
    double xPos;
    double yPos;
    double kVal;
} Point2D;

// 三维点坐标
typedef struct Point_3D
{
    double xPos;
    double yPos;
    double zPos;
    double kVal; // 斜率值
} Point3D;

/////////////////////////////////////////LINE_2D/////////////////////////////////////////////
// 定义二阶矩阵结构
typedef struct Matrix2D
{
    double matrixElem[2][2];
} Matrix2D;

//////////////////////////////////////////Matrix3D////////////////////////////////////////////
// 定义三阶矩阵结构
typedef struct Matrix3D
{
    double matrixElem[3][3];
} Matrix3D;

// 定义一个3x3矩阵
typedef std::vector<std::vector<double>> Matrix3DVec;

#endif