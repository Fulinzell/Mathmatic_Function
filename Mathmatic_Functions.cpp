#include "Mathmatic_Functions.h"

// Chudnovsky 级数任意精度计算所需的多精度浮点类型 (约 101 位有效数字)
#include <boost/multiprecision/cpp_dec_float.hpp>

/********************************************************************************************************************************************************

                                                             二维点计算函数汇总

*********************************************************************************************************************************************************/
// （欧拉几何）计算任意两点所构成的斜率
double calculateSlope(const POINT_2D &point1, const POINT_2D &point2)
{
    if (point2.X_Pos - point1.X_Pos == 0)
    {
        throw std::invalid_argument("Cannot calculate slope: division by zero.");
    }
    return (point2.Y_Pos - point1.Y_Pos) / (point2.X_Pos - point1.X_Pos);
}

// 获取任意两点所构成的线段所占的弧度
double calculateArc(const POINT_2D &point1, const POINT_2D &point2)
{
    double slope = calculateSlope(point1, point2);
    return atan(slope);
}

/********************************************************************************************************************************************************

                                                             三维点计算函数汇总

*********************************************************************************************************************************************************/
// 计算任意两点构成线段的方向余弦(三维点坐标)
void calculateDirectionCosines(const POINT_3D &point1, const POINT_3D &point2, double &cosAlpha, double &cosBeta, double &cosGamma)
{
    // 计算方向向量的分量
    double deltaX = point2.X_Pos - point1.X_Pos;
    double deltaY = point2.Y_Pos - point1.Y_Pos;
    double deltaZ = point2.Z_Pos - point1.Z_Pos;

    // 计算方向向量的长度
    double length = std::sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

    // 检查是否为零向量
    if (length == 0)
    {
        throw std::invalid_argument("Cannot calculate direction cosines: zero-length vector.");
    }

    // 计算方向余弦
    cosAlpha = deltaX / length; // X方向的方向余弦
    cosBeta = deltaY / length;  // Y方向的方向余弦
    cosGamma = deltaZ / length; // Z方向的方向余弦
}

/********************************************************************************************************************************************************

                                                      阶乘 / 伽马函数 / 斯特林公式

*********************************************************************************************************************************************************/
// 圆周率常量（避免依赖 C++20 的 std::numbers）
static const double PI = 3.14159265358979323846264338327950288;

// 计算任意非负整数的阶乘
/*
    输入：@param n 任意非负整数（无上限）
    输出：返回 n 的阶乘的十进制字符串（如 25! = "15511210043330985984000000"）
    算法：十进制逐位乘法 —— 用 vector 存储低位在前的十进制数字，
          从 2 乘到 n，每轮对每一位乘以因子并处理进位，最后逆序拼接为字符串
    复杂度：O(n × 位数) ≈ O(n²) 量级（朴素大数乘法）
*/
std::string Factorial(unsigned int n)
{
    // 0! = 1! = 1
    if (n <= 1)
    {
        return "1";
    }

    // digits 存放十进制数字，下标 0 为最低位，初始值 1 表示数字 "1"
    std::vector<int> digits(1, 1);

    for (unsigned int factor = 2; factor <= n; ++factor)
    {
        int carry = 0; // 进位
        for (size_t i = 0; i < digits.size(); ++i)
        {
            int product = digits[i] * factor + carry; // 当前位 × 因子 + 进位
            digits[i] = product % 10;                 // 保留个位
            carry = product / 10;                     // 其余部分进位
        }
        // 将剩余进位逐位追加到高位
        while (carry > 0)
        {
            digits.push_back(carry % 10);
            carry /= 10;
        }
    }

    // 逆序拼接为十进制字符串（高位在前）
    std::string result;
    result.reserve(digits.size());
    for (auto it = digits.rbegin(); it != digits.rend(); ++it)
    {
        result.push_back(static_cast<char>('0' + *it));
    }
    return result;
}

// 伽马函数(实数域)
/*
    输入：@param x 任意实数（0 与负整数为极点, 抛 std::invalid_argument）
    输出：返回 Γ(x) 的近似值（double 精度, 相对误差 ~1e-12）
    算法：x ≥ 0.5 时使用 Lanczos 近似(g = 7, n = 9)；
          x < 0.5 时使用欧拉反射公式 Γ(x) = π / (sin(πx) · Γ(1 - x))
    备注：阶乘与伽马的关系 n! = Γ(n + 1)，小数阶乘 x! = Γ(x + 1)，
          如 4.5! = Γ(5.5) ≈ 52.34277778455352
*/
double Gamma_Function(double x)
{
    // 极点检测：0 与负整数处 Γ 发散
    if (x <= 0.0 && std::floor(x) == x)
    {
        throw std::invalid_argument("Gamma function pole: x must not be 0 or a negative integer.");
    }

    // 反射公式：覆盖 x < 0 的非整数以及 (0, 0.5) 区间（避免 Lanczos 在小值处精度下降）
    if (x < 0.5)
    {
        return PI / (std::sin(PI * x) * Gamma_Function(1.0 - x));
    }

    // Lanczos 近似（g = 7, n = 9，系数来自经典 Lanczos 系数表）
    static const double lanczosCoeff[9] = {
        0.99999999999980993,
        676.5203681218851,
        -1259.1392167224028,
        771.32342877765313,
        -176.61502916214059,
        12.507343278686905,
        -0.13857109526572012,
        9.9843695780195716e-6,
        1.5056327351493116e-7
    };

    const double g = 7.0;
    x -= 1.0; // 以 Γ(x + 1) 形式展开

    double a = lanczosCoeff[0];
    double t = x + g + 0.5;
    for (int i = 1; i < 9; ++i)
    {
        a += lanczosCoeff[i] / (x + i);
    }

    return std::sqrt(2.0 * PI) * std::pow(t, x + 0.5) * std::exp(-t) * a;
}

// 斯特林公式近似 n!
/*
    输入：@param n 非负整数
    输出：返回 n! 的近似值（double, n ≤ 170 时在 double 范围内有效）
    公式：n! ≈ √(2πn) · (n/e)^n · (1 + 1/(12n))   —— 含一阶修正项
    备注：n = 0 特判返回 1（0! = 1）；n 越大相对误差越小（~1/(288n²)）
*/
double Stirling_Approximation(unsigned int n)
{
    if (n == 0)
    {
        return 1.0; // 斯特林公式对 0 无定义, 0! = 1 特判
    }
    // 经对数形式计算, 避免中间量 (n/e)^n 先于最终结果溢出
    return std::exp(Stirling_Ln(n));
}

// 斯特林公式对数形式 ln(n!)
/*
    输入：@param n 非负整数
    输出：返回 ln(n!) 的近似值（double, 任意 n 不溢出）
    公式：ln(n!) ≈ n·ln(n) − n + 0.5·ln(2πn) + 1/(12n) − 1/(360n³) + 1/(1260n⁵)
    备注：n = 0 返回 ln(1) = 0；n ≥ 10 时相对误差已小于 1e-12 量级
*/
double Stirling_Ln(unsigned int n)
{
    if (n == 0)
    {
        return 0.0; // ln(0!) = ln(1) = 0
    }

    double dn = static_cast<double>(n);
    double n2 = dn * dn;
    double n3 = n2 * dn;
    double n5 = n3 * n2;

    return dn * std::log(dn) - dn + 0.5 * std::log(2.0 * PI * dn)
         + 1.0 / (12.0 * dn) - 1.0 / (360.0 * n3) + 1.0 / (1260.0 * n5);
}

/********************************************************************************************************************************************************

                                                      Chudnovsky 级数计算圆周率

*********************************************************************************************************************************************************/
/*
    Chudnovsky 级数 (1988, 拉马努金级数的推广):

                  ∞
        1         Σ  (-1)^k (6k)! (13591409 + 545140134 k)
        ── = 12 · ──────────────────────────────────────────
        π         k=0   (3k)! (k!)^3  640320^(3k + 3/2)

    等价计算形式 (避免分数次幂):
                  426880 · √10005
        π = ────────────────────────────────────────────────
              Σ  (-1)^k (6k)! (13591409 + 545140134 k) / ((3k)! (k!)^3 640320^(3k))

    相邻项递推 (每一项只做常数次乘除, 总时间 O(N)):
        T_0 = 13591409
        T_{k+1} = T_k · [-8·(6k+1)(6k+3)(6k+5)] / [(k+1)^3 · 640320³] · (A + B(k+1))/(A + Bk)
        推导: (6k+2)/(3k+1) = (6k+4)/(3k+2) = (6k+6)/(3k+3) = 2, 三个因子约去后余 2³ = 8
    收敛速度: 每加一项约增加 14.18 位十进制正确数字。
*/

namespace
{
    const double CHUD_A = 13591409.0;        // 常数项 A
    const double CHUD_B = 545140134.0;       // 线性系数 B
    // 640320³ / 8 = 32817176580096000 (略超 2^53, double 有 ~1 ulp 舍入, 对结果无影响)
    const double CHUD_C = 32817176580096000.0;
}

// Chudnovsky 级数计算 π (double 精度)
/*
    输入：@param terms 累加项数 (≥ 1; 每项约多 14 位正确数字, 2 项即达 double 上限)
    输出：返回 π 的 double 近似值
*/
double Chudnovsky_Pi(unsigned int terms)
{
    if (terms == 0)
    {
        throw std::invalid_argument("Chudnovsky_Pi: terms must be at least 1.");
    }

    double term = CHUD_A; // T_0
    double sum  = CHUD_A; // 级数部分和
    for (unsigned int k = 0; k + 1 < terms; ++k)
    {
        double dk = static_cast<double>(k);
        // 阶乘比值部分: 8·(6k+1)(6k+3)(6k+5) / [(k+1)^3 · 640320³]
        double ratio = (6.0 * dk + 1.0) * (6.0 * dk + 3.0) * (6.0 * dk + 5.0)
                     / (CHUD_C * (dk + 1.0) * (dk + 1.0) * (dk + 1.0));
        // 线性系数部分: (A + B(k+1)) / (A + Bk)
        ratio *= (CHUD_A + CHUD_B * (dk + 1.0)) / (CHUD_A + CHUD_B * dk);
        term *= -ratio; // 交错符号 (-1)^k
        sum  += term;
    }

    double pi = 426880.0 * std::sqrt(10005.0) / sum;

    // 文件输出：执行时将结果写入 函数名_参数值_Bits.md（默认当前目录, 可用 setPiOutputDirectory 指定）
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(17) << pi;
    savePiResultToFile("Chudnovsky_Pi_" + std::to_string(terms) + "_Bits.md",
                       "Chudnovsky_Pi (double)", "terms = " + std::to_string(terms), oss.str());

    return pi;
}

// Chudnovsky 级数计算 π (任意精度, 返回十进制字符串)
/*
    输入：@param digits 期望有效数字位数 (1 ~ 100)
          @param terms  累加项数; 传 0 按 digits/14 + 2 自动选取 (留一位余量)
    输出：返回 π 的十进制字符串, 保留 digits 位小数
    内部：使用 boost::multiprecision::cpp_dec_float_100 (101 位十进制有效数字)
*/
std::string Chudnovsky_Pi(unsigned int digits, unsigned int terms)
{
    if (digits == 0 || digits > 100)
    {
        throw std::invalid_argument("Chudnovsky_Pi: digits must be in range 1 ~ 100.");
    }
    if (terms == 0)
    {
        terms = digits / 14 + 2; // 每项约 14.18 位, 多加 2 项保险
    }

    using boost::multiprecision::cpp_dec_float_100;
    static const cpp_dec_float_100 A("13591409");
    static const cpp_dec_float_100 B("545140134");
    static const cpp_dec_float_100 C("32817176580096000"); // 640320³ / 8
    static const cpp_dec_float_100 NUM("426880");
    static const cpp_dec_float_100 RADICAND("10005");

    cpp_dec_float_100 term = A; // T_0
    cpp_dec_float_100 sum  = A;
    for (unsigned int k = 0; k + 1 < terms; ++k)
    {
        cpp_dec_float_100 dk(k);
        cpp_dec_float_100 ratio = (6 * dk + 1) * (6 * dk + 3) * (6 * dk + 5)
                                / (C * (dk + 1) * (dk + 1) * (dk + 1));
        ratio *= (A + B * (dk + 1)) / (A + B * dk);
        term *= -ratio;
        sum  += term;
    }

    cpp_dec_float_100 pi = NUM * boost::multiprecision::sqrt(RADICAND) / sum;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(digits) << pi;
    std::string piStr = oss.str();

    // 文件输出：执行时将结果写入 函数名_参数值_Bits.md（默认当前目录, 可用 setPiOutputDirectory 指定）
    savePiResultToFile("Chudnovsky_Pi_" + std::to_string(digits) + "_Bits.md",
                       "Chudnovsky_Pi (high precision)",
                       "digits = " + std::to_string(digits) + ", terms = " + std::to_string(terms), piStr);

    return piStr;
}