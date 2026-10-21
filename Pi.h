#ifndef __PI_H__
#define __PI_H__

#include <string>

//////////////////////////////////////////////////////// 圆周率计算相关函数

// 拉马努金级数计算 π（1914, double 精度）
// 公式：1/π = (2√2 / 9801) · Σ (4k)!·(1103 + 26390k) / ((k!)^4 · 396^(4k))
// 每项约增加 8 位十进制精度，terms = 2~3 即达到 double 精度极限
// @param terms 求和项数（>= 1）
// @return π 的 double 近似值
double calculatePi_Ramanujan(int terms);

// Chudnovsky 级数计算 π 的任意位数（1989, 拉马努金类级数, 每项约 14.18 位）
// 公式：π = 426880·√10005 / Σ (-1)^k·(6k)!·(13591409 + 545140134k) / ((3k)!·(k!)^3·640320^(3k))
// 内部使用任意精度整数算术（binary splitting + 大整数开方/除法），
// 输出小数点后 digits 位全部为 π 的真实十进制位（逐位正确, 非浮点近似）
// @param digits 小数点后位数（>= 1）
// @return 形如 "3.14159..." 的字符串（小数点后 digits 位）
std::string calculatePi_Chudnovsky(int digits);

//////////////////////////////////////////////////////// π 结果文件输出

// 设置 π 计算结果输出目录（默认当前工作目录 "."）
// @param dir 目录路径（可相对可绝对; 目录需已存在）
void setPiOutputDirectory(const std::string &dir);

// 将 π 计算结果保存为 Markdown 文件（文件名 = 函数名_参数值_Bits.md）
// @param filename  文件名, 如 "calculatePi_Chudnovsky_500_Bits.md"
// @param funcName  函数名（写入文件元信息）
// @param paramDesc 参数描述, 如 "digits = 500"（写入文件元信息）
// @param result    计算结果字符串
// @return 保存的完整文件路径
// @throw  std::runtime_error 文件无法打开/写入时抛出
std::string savePiResultToFile(const std::string &filename, const std::string &funcName,
                               const std::string &paramDesc, const std::string &result);

#endif // __PI_H__
