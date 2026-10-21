#include "Binary.h"

#include <algorithm> // std::reverse
#include <stdexcept>

/********************************************************************************************************************************************************

                                                        十进制 → 二进制（字符串拼接通用算法）

*********************************************************************************************************************************************************/
/*
    算法说明（全程字符串运算，不经过浮点）：

    1) 整数部分 —— 除 2 取余：
       对十进制字符串反复做"字符串除法"（逐位除 2，余数留给下一位），
       每轮得到的余数（0 或 1）追加到结果串，直到商为 "0"；
       结果串逆序即为二进制整数部分。

    2) 小数部分 —— 乘 2 取整：
       对十进制小数字符串反复做"字符串乘法"（逐位乘 2 并处理进位），
       乘积的第一位数字即为下一个二进制位（0 或 1），剩余部分作为新的小数继续；
       直到小数部分归零（精确表示）或达到位数上限（无法精确表示）。

    3) 符号与小数点按位置拼接：  "[-]整数部分[.小数部分]"
*/

namespace
{
    // 去除字符串首尾的空白字符
    std::string trim(const std::string &s)
    {
        size_t b = s.find_first_not_of(" \t\r\n");
        if (b == std::string::npos) return "";
        size_t e = s.find_last_not_of(" \t\r\n");
        return s.substr(b, e - b + 1);
    }

    // 校验数字字符串（不含符号），允许空串（视为 "0"）
    void validateDigits(const std::string &s)
    {
        for (char c : s)
        {
            if (c < '0' || c > '9')
            {
                throw std::invalid_argument("Decimal_To_Binary: invalid character in input.");
            }
        }
    }

    // 去除前导零："000123" -> "123"，全零 -> "0"
    std::string stripLeadingZeros(const std::string &s)
    {
        size_t p = s.find_first_not_of('0');
        return (p == std::string::npos) ? "0" : s.substr(p);
    }

    // 判断数字字符串是否含有非零位（用于小数部分的全零判断）
    bool hasNonZeroDigit(const std::string &s)
    {
        return s.find_first_not_of('0') != std::string::npos;
    }

    // 十进制字符串 ÷ 2，返回商；余数通过 rem 带出
    std::string divDecimalBy2(const std::string &num, int &rem)
    {
        std::string quotient;
        quotient.reserve(num.size());
        int carry = 0;
        for (char c : num)
        {
            int cur = carry * 10 + (c - '0');
            quotient.push_back(static_cast<char>('0' + cur / 2));
            carry = cur % 2;
        }
        rem = carry;
        return stripLeadingZeros(quotient);
    }

    // 十进制小数字符串 × 2：
    // 小数位数不变时，乘积整体仍是小数（整数位为 0）；
    // 只有发生进位（乘积多出一位）时，多出的首位才是整数位 1。
    // 返回新的十进制小数字符串（与原串等长），整数位通过 topDigit 带出（0 或 1）
    std::string mulDecimalBy2(const std::string &frac, int &topDigit)
    {
        std::string product; // 低位在前的数字序列
        product.reserve(frac.size() + 1);
        int carry = 0;
        for (auto it = frac.rbegin(); it != frac.rend(); ++it)
        {
            int cur = (*it - '0') * 2 + carry;
            product.push_back(static_cast<char>('0' + cur % 10));
            carry = cur / 10;
        }
        topDigit = carry; // 最高位进位即乘 2 后的整数位（只可能是 0 或 1）
        std::reverse(product.begin(), product.end());
        return product; // 位数与原小数相同
    }
} // namespace

std::string Decimal_To_Binary(const std::string &num, unsigned int maxFracBits)
{
    std::string s = trim(num);
    if (s.empty())
    {
        throw std::invalid_argument("Decimal_To_Binary: empty input.");
    }

    // 1) 解析符号
    bool negative = false;
    if (s[0] == '-' || s[0] == '+')
    {
        negative = (s[0] == '-');
        s = s.substr(1);
        if (s.empty())
        {
            throw std::invalid_argument("Decimal_To_Binary: sign without digits.");
        }
    }

    // 2) 拆分整数 / 小数
    size_t dot = s.find('.');
    if (dot != std::string::npos && s.find('.', dot + 1) != std::string::npos)
    {
        throw std::invalid_argument("Decimal_To_Binary: multiple decimal points.");
    }
    std::string intPart  = (dot == std::string::npos) ? s : s.substr(0, dot);
    std::string fracPart = (dot == std::string::npos) ? ""  : s.substr(dot + 1);

    validateDigits(intPart);
    validateDigits(fracPart);
    if (intPart.empty())  intPart  = "0";
    if (fracPart.empty()) fracPart = "0";

    intPart  = stripLeadingZeros(intPart);
    // 注意：小数部分不能去前导零 —— "0.0005" 的 "0005" 每一位都携带 10 的负幂次信息，
    // 去掉前导零会改变数值（0.0005 ≠ 0.5）。是否为零改用"全零"判断。

    std::string result = negative ? "-" : "";

    // 3) 整数部分：除 2 取余，逆序拼接
    if (intPart == "0")
    {
        result += "0";
    }
    else
    {
        std::string bits; // 低位在前收集
        while (intPart != "0")
        {
            int rem = 0;
            intPart = divDecimalBy2(intPart, rem);
            bits.push_back(static_cast<char>('0' + rem));
        }
        result.append(bits.rbegin(), bits.rend()); // 逆序：高位在前
    }

    // 4) 小数部分：乘 2 取整；全零则无小数部分
    if (hasNonZeroDigit(fracPart) && maxFracBits > 0)
    {
        std::string fracBits;
        for (unsigned int i = 0; i < maxFracBits && hasNonZeroDigit(fracPart); ++i)
        {
            int topDigit = 0;
            fracPart = mulDecimalBy2(fracPart, topDigit); // 位数恒定，保持 10 的负幂次尺度
            fracBits.push_back(static_cast<char>('0' + topDigit));
        }
        if (!fracBits.empty())
        {
            result += "." + fracBits;
            if (hasNonZeroDigit(fracPart))
            {
                result += "…1"; // 位数用尽仍未精确表示：标记后续仍有非零位
            }
        }
    }

    return result;
}
