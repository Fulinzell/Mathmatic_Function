#pragma once

#include <string>

//////////////////////////////////////////////////////// 进制转换相关函数

// 将任意十进制数（整数或小数）转换为二进制字符串表示
// 全程使用字符串运算（字符拼接），不经过 double，无浮点精度损失
// @param num          十进制数字符串，可带负号与小数点，如 "25"、"-25"、"3.14159"
// @param maxFracBits  小数部分最多保留的二进制位数（默认 52，约 double 尾数精度；
//                     小数部分乘 2 取整后若提前归零则自然截断）
// @return 二进制字符串，形如 "11001"、"-11001.001"、"11.001001..."（无法精确表示时末位标 1 表示后续仍有位）
// @throw  std::invalid_argument 输入为空、含非法字符、或同时出现多个小数点时抛出
std::string Decimal_To_Binary(const std::string &num, unsigned int maxFracBits = 52);
