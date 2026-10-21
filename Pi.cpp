#include "Pi.h"

#include <boost/multiprecision/cpp_int.hpp> // 任意精度整数, 支撑 Chudnovsky 真值计算
#include <cmath>
#include <ctime>    // 时间戳
#include <fstream>  // 文件输出
#include <iomanip>  // setprecision
#include <sstream>  // ostringstream
#include <stdexcept>
#include <string>

using boost::multiprecision::cpp_int;

/********************************************************************************************************************************************************

                                                             圆周率计算函数汇总

*********************************************************************************************************************************************************/

namespace
{
//////////////////////////////////////////////////////// 内部：Chudnovsky 任意精度支持（不对外暴露）////////////////////////////////////////////////////////

// 常量：640320^3 / 24（binary splitting 中 q 项的固定因子）
const cpp_int C3_OVER_24 = cpp_int(640320) * 640320 * 640320 / 24;

// Chudnovsky 级数 binary splitting（二分求和）
/*
    计算区间 [a, b) 的 P/Q/T 三元组，满足：
      S(a,b) = Σ_{k=a}^{b-1} (-1)^k·(6k)!·(13591409 + 545140134k) / ((3k)!·(k!)^3·640320^(3k))
             = T(a,b) / Q(a,b)
    叶子项：p_k = (6k-5)(2k-1)(6k-1)，q_k = k³·640320³/24
    合并式：P = P1·P2，Q = Q1·Q2，T = T1·Q2 + P1·T2
*/
void chudnovskyBS(int a, int b, cpp_int &P, cpp_int &Q, cpp_int &T)
{
    if (b - a == 1)
    {
        if (a == 0)
        {
            P = 1;
            Q = 1;
            T = 13591409; // term_0 = (0)!·(13591409 + 0) / 1 = 13591409
            return;
        }
        // 注意：先转 cpp_int 再相乘, 避免 int 乘法溢出
        cpp_int pa = cpp_int(6) * a - 5;   // (6a-5)
        cpp_int pb = cpp_int(2) * a - 1;   // (2a-1)
        cpp_int pc = cpp_int(6) * a - 1;   // (6a-1)
        P = pa * pb * pc;                  // p_a

        Q = cpp_int(a) * a * a * C3_OVER_24; // q_a = a³·640320³/24

        T = P * (13591409 + cpp_int(545140134) * a); // t_a = p_a·(13591409 + 545140134a)
        if (a & 1)
        {
            T = -T; // 奇数项符号为负
        }
        return;
    }

    int m = (a + b) / 2;
    cpp_int P1, Q1, T1, P2, Q2, T2;
    chudnovskyBS(a, m, P1, Q1, T1);
    chudnovskyBS(m, b, P2, Q2, T2);

    P = P1 * P2;
    Q = Q1 * Q2;
    T = T1 * Q2 + P1 * T2;
}

// 大整数平方根（向下取整）：牛顿迭代 x_{k+1} = (x + n/x) / 2
cpp_int isqrtBig(const cpp_int &n)
{
    if (n <= 0)
    {
        return 0;
    }

    // 初始估计：取 n 的前 15 位十进制用 double 估算, 再补回量级, 保证初始值 >= sqrt(n)
    std::string ns = n.convert_to<std::string>();
    long double lead = std::stold(ns.substr(0, 15));
    long double est = std::sqrt(lead);
    cpp_int x = cpp_int(est) + 1; // 向上取整, 防低估
    int half = (static_cast<int>(ns.size()) - 1) / 2;
    while (half-- > 0)
    {
        x *= 10;
    }

    // 牛顿迭代（初始值偏大, 单调递减收敛）
    for (int i = 0; i < 64; ++i)
    {
        cpp_int nx = (x + n / x) / 2;
        if (nx >= x)
        {
            break; // 已收敛
        }
        x = nx;
    }

    // 精确校正：确保 x² <= n < (x+1)²
    while (x * x > n)
    {
        x -= 1;
    }
    while ((x + 1) * (x + 1) <= n)
    {
        x += 1;
    }
    return x;
}

} // namespace

/********************************************************************************************************************************************************

                                                             拉马努金级数（double 精度）

*********************************************************************************************************************************************************/
// 拉马努金级数计算 π（1914）
/*
    输入：@param terms 求和项数（>= 1）
    输出：返回 π 的 double 近似值
    公式：1/π = (2√2 / 9801) · Σ_{k=0}^{∞} (4k)!·(1103 + 26390k) / ((k!)^4 · 396^(4k))
    精度：每项约增加 8 位十进制精度；terms = 2~3 即达到 double 精度极限（~15 位）
*/
double piRamanujan(int terms)
{
    if (terms <= 0)
    {
        throw std::invalid_argument("piRamanujan: terms must be >= 1.");
    }

    double sum = 0.0;
    for (int k = 0; k < terms; ++k)
    {
        // 计算 (4k)!
        double fact4k = 1.0;
        for (int i = 1; i <= 4 * k; ++i)
        {
            fact4k *= i;
        }
        // 计算 k!
        double factK = 1.0;
        for (int i = 1; i <= k; ++i)
        {
            factK *= i;
        }
        // 级数项：term_k = (4k)!·(1103 + 26390k) / ((k!)^4 · 396^(4k))
        double term = fact4k * (1103.0 + 26390.0 * k) / (std::pow(factK, 4) * std::pow(396.0, 4 * k));
        sum += term;
    }

    // 1/π = (2√2/9801)·sum  =>  π = 9801 / (2√2·sum)
    double pi = 9801.0 / (2.0 * std::sqrt(2.0) * sum);

    // 文件输出：执行时将结果写入 函数名_参数值_Bits.md（默认当前目录, 可用 setPiOutputDirectory 指定）
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(17) << pi;
    savePiResultToFile("piRamanujan_" + std::to_string(terms) + "_Bits.md",
                       "piRamanujan", "terms = " + std::to_string(terms), oss.str());

    return pi;
}

/********************************************************************************************************************************************************

                                                             Chudnovsky 级数（任意精度真值）

*********************************************************************************************************************************************************/
// Chudnovsky 级数计算 π 的任意位数（1989）
/*
    输入：@param digits 小数点后位数（>= 1）
    输出：返回形如 "3.14159..." 的字符串（小数点后 digits 位, 全部为真值）
    公式：π = 426880·√10005 / Σ_{k=0}^{∞} (-1)^k·(6k)!·(13591409 + 545140134k) / ((3k)!·(k!)^3·640320^(3k))
    精度：每项约贡献 14.181647462725477 位十进制（拉马努金类级数收敛最快者之一）
    流程：
      1) 项数估算：terms = ceil(digits / 14.1816...) + 2（安全余量保证截断误差远小于最后一位）
      2) binary splitting 精确计算 S = T/Q（全程任意精度整数, 无舍入误差）
      3) 定点开方：sqrtC = floor(√(10005·10^(2·digits))) ≈ √10005·10^digits
      4) 大整数除法：piScaled = floor(426880·√10005·Q·10^digits / T) = floor(π·10^digits)
      5) 转字符串并插入小数点
    说明：全程整数运算, 输出位即为 π 的真实十进制展开（每一位都是真值）
*/
std::string piChudnovsky(int digits)
{
    if (digits <= 0)
    {
        throw std::invalid_argument("piChudnovsky: digits must be >= 1.");
    }

    // 每项约 14.181647462725477 位；+2 项安全余量
    const int terms = static_cast<int>(digits / 14.181647462725477) + 2;

    // 精确求和 S = T/Q
    cpp_int P, Q, T;
    chudnovskyBS(0, terms, P, Q, T);
    if (T < 0)
    {
        throw std::runtime_error("piChudnovsky: unexpected negative T."); // 防御性检查, 理论上不会发生
    }

    // sqrtC = floor(√(10005 · 10^(2·digits))) ≈ √10005 · 10^digits
    cpp_int one = 1;
    for (int i = 0; i < digits; ++i)
    {
        one *= 10;
    }
    cpp_int sqrtC = isqrtBig(cpp_int(10005) * one * one);

    // piScaled = floor(426880 · √10005 · 10^digits · Q / T) = floor(π · 10^digits)
    cpp_int piScaled = (Q * 426880 * sqrtC) / T;

    // 转字符串并插入小数点（首位应为 '3'）
    std::string s = piScaled.convert_to<std::string>();
    if (static_cast<int>(s.size()) < digits + 1)
    {
        s = std::string(digits + 1 - s.size(), '0') + s; // 防御性前补零
    }
    std::string piStr = s.substr(0, 1) + "." + s.substr(1, digits);

    // 文件输出：执行时将结果写入 函数名_参数值_Bits.md（默认当前目录, 可用 setPiOutputDirectory 指定）
    savePiResultToFile("piChudnovsky_" + std::to_string(digits) + "_Bits.md",
                       "piChudnovsky", "digits = " + std::to_string(digits), piStr);

    return piStr;
}

/********************************************************************************************************************************************************

                                                             π 结果文件输出

*********************************************************************************************************************************************************/
namespace
{
    std::string g_piOutputDir = "."; // 默认当前工作目录
}

// 设置 π 计算结果输出目录
void setPiOutputDirectory(const std::string &dir)
{
    g_piOutputDir = dir;
}

// 将 π 计算结果保存为 Markdown 文件
std::string savePiResultToFile(const std::string &filename, const std::string &funcName,
                               const std::string &paramDesc, const std::string &result)
{
    // 组装文件路径（'/' 在 Windows 与 POSIX 下均可用）
    std::string path = filename;
    if (!g_piOutputDir.empty() && g_piOutputDir != ".")
    {
        path = g_piOutputDir + "/" + filename;
    }

    // 组装 Markdown 内容：元信息表 + 结果代码块
    std::time_t now = std::time(nullptr);
    char timeBuf[32];
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::ostringstream md;
    md << "# π 计算结果\n\n"
       << "| 项 | 值 |\n"
       << "|----|----|\n"
       << "| 函数 | `" << funcName << "` |\n"
       << "| 参数 | " << paramDesc << " |\n"
       << "| 结果长度 | " << result.size() << " 字符 |\n"
       << "| 生成时间 | " << timeBuf << " |\n\n"
       << "## 计算结果\n\n"
       << "```text\n" << result << "\n```\n";

    std::ofstream out(path);
    if (!out)
    {
        throw std::runtime_error("savePiResultToFile: cannot open file " + path);
    }
    out << md.str();
    out.close();
    return path;
}
