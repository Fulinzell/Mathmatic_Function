#pragma once

#include "Head.h"

//////////////////////////////////////////////////////// 数学计算相关函数

// 计算任意两点所构成的斜率(二维点坐标)
double calculateSlope(const POINT_2D &point1, const POINT_2D &point2);

// 获取任意两点所构成的线段所占的弧度(二维点坐标)
double calculateArc(const POINT_2D &point1, const POINT_2D &point2);

// 计算三维点之间的斜率(三维点坐标)
void calculateDirectionCosines(const POINT_3D &point1, const POINT_3D &point2, double &cosAlpha, double &cosBeta, double &cosGamma);

//////////////////////////////////////////////////////// 阶乘 / 伽马函数 / 斯特林公式

// 计算任意非负整数的阶乘(返回十进制字符串, 支持任意大的 n)
std::string Factorial(unsigned int n);

// 伽马函数(实数域, 支持小数阶乘 x! = Γ(x+1); 0 与负整数为极点, 抛 std::invalid_argument)
double Gamma_Function(double x);

// 斯特林公式近似 n! (double 范围, n ≤ 170 有效, 含一阶修正项)
double Stirling_Approximation(unsigned int n);

// 斯特林公式对数形式 ln(n!) (任意 n 不溢出, 含三阶修正项)
double Stirling_Ln(unsigned int n);

//////////////////////////////////////////////////////// Chudnovsky 级数计算圆周率

// Chudnovsky 级数计算 π (double 精度, 每加一项约多 14 位正确数字, 2 项即达 double 上限)
double Chudnovsky_Pi(unsigned int terms);

// Chudnovsky 级数计算 π (任意精度, 返回十进制字符串, 最多支持 100 位有效数字)
// @param digits 期望的有效数字位数 (1 ~ 100)
// @param terms  累加项数; 传 0 表示按 digits/14 + 2 自动选择
std::string Chudnovsky_Pi(unsigned int digits, unsigned int terms);
