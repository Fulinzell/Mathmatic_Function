# Mathmatic_Function

一个 C++ 数学计算库：解析几何、行列式、任意大整数阶乘、伽马函数、斯特林公式，以及基于 Chudnovsky / Ramanujan 级数的圆周率高精度计算（支持百万位级真值输出）。

## ✨ 功能特性

| 模块 | 内容 | 亮点 |
|------|------|------|
| 解析几何 | 二维斜率、线段弧度、三维方向余弦 | 除零/零向量防护，抛 `std::invalid_argument` |
| 矩阵运算 | 3×3 行列式计算、格式化打印 | 支持 `struct` 与 `vector<vector<double>>` 两种矩阵类型 |
| 阶乘 | 任意非负整数阶乘 | 返回十进制字符串，无溢出上限（大数逐位乘法） |
| 伽马函数 | 实数域 Γ(x)，支持小数阶乘 | Lanczos 近似 + 欧拉反射公式，相对误差 ~1e-12 |
| 斯特林公式 | n! 近似与 ln(n!) | 含高阶修正项，任意 n 不溢出 |
| 圆周率 π | Ramanujan 级数（double） | 每项约 +8 位，2~3 项达 double 上限 |
| 圆周率 π | Chudnovsky 级数（高精度） | 每项约 +14.18 位；100 位内用 `cpp_dec_float_100`，任意位数用纯整数 binary splitting，逐位真值 |
| 结果输出 | π 计算结果自动写 Markdown 文件 | `setPiOutputDirectory` 可指定输出目录 |

## 🚀 快速开始

### 环境要求

- Windows + MinGW-w64（GCC）
- [Boost](https://www.boost.org/)（仅需头文件，`boost/multiprecision`）

### 编译运行

```bash
mingw32-make        # 编译生成 Main.exe
mingw32-make run    # 清理后重新编译并运行（含耗时显示）
mingw32-make clean  # 清理目标文件
```

## 📁 目录结构

```
Mathmatic_Function/
├── Main.cpp                  # 示例程序（全部 API 的使用演示）
├── Head.h                    # 统一头文件
├── Mathmatic_DataType.h      # POINT_2D / POINT_3D / MATRIX2D / MATRIX3D 等类型定义
├── Mathmatic_Functions.h/.cpp# 斜率、方向余弦、阶乘、伽马、斯特林、Chudnovsky(double/高精度)
├── Matrix.h/.cpp             # 行列式计算与矩阵打印
├── Pi.h/.cpp                 # Ramanujan 级数、整数版 Chudnovsky（任意位真值）、结果文件输出
├── Thread.h                  # 线程/网络相关头文件（预留）
├── makefile                  # GCC/Windows 构建脚本
├── 函数功能详解.md            # 各函数原理与公式推导
├── 调试日志.md               # 开发调试记录
└── 需求与愿景.md             # 项目规划
```

## 📖 API 速览

```cpp
// 解析几何
double calculateSlope(const POINT_2D &p1, const POINT_2D &p2);                    // 斜率（两点 x 相同抛异常）
double calculateArc(const POINT_2D &p1, const POINT_2D &p2);                      // 线段弧度
void   calculateDirectionCosines(const POINT_3D &p1, const POINT_3D &p2,
                                 double &cosA, double &cosB, double &cosG);      // 方向余弦

// 阶乘 / 伽马 / 斯特林
std::string Factorial(unsigned int n);            // 任意大 n 的阶乘，十进制字符串
double Gamma_Function(double x);                  // Γ(x)，0 与负整数为极点（抛异常）
double Stirling_Approximation(unsigned int n);    // n! 近似（n ≤ 170）
double Stirling_Ln(unsigned int n);               // ln(n!)，任意 n 不溢出

// 圆周率
double      Chudnovsky_Pi(unsigned int terms);              // double 精度，每项 +14 位
std::string Chudnovsky_Pi(unsigned int digits, unsigned int terms = 0);  // 1~100 位，字符串
double      calculatePi_Ramanujan(int terms);               // double 精度，每项 +8 位
std::string calculatePi_Chudnovsky(int digits);             // 任意位真值（整数 binary splitting）

// 输出控制
void setPiOutputDirectory(const std::string &dir);  // 指定 *_Bits.md 输出目录
```

## 🔬 算法说明

- **大数阶乘**：`vector` 存储低位在前十进制数字，逐因子乘法 + 进位，O(n²) 量级。
- **伽马函数**：x ≥ 0.5 用 Lanczos 近似（g = 7, n = 9）；x < 0.5 用欧拉反射公式 Γ(x) = π / (sin(πx)·Γ(1−x))。阶乘关系：x! = Γ(x+1)。
- **Chudnovsky 级数**（1988，拉马努金级数推广）：

  ```
  π = 426880·√10005 / Σ (-1)^k (6k)! (13591409 + 545140134k) / ((3k)! (k!)^3 640320^(3k))
  ```

  相邻项线性递推，每加一项约多 14.18 位正确数字。任意精度版采用 **binary splitting（二分求和）**：全程大整数运算无舍入误差，配合牛顿迭代大整数开方，输出小数点后每一位都是 π 的真实十进制位。
- **斯特林公式**：ln(n!) ≈ n·ln n − n + ½·ln(2πn) + 1/(12n) − 1/(360n³) + 1/(1260n⁵)。

## ⚠️ 已知限制

- 仅支持 Windows / MinGW（`Head.h` 依赖 `windows.h`、`winsock2.h`）。
- `Chudnovsky_Pi` 高精度版上限 100 位；更多位请用 `calculatePi_Chudnovsky`。
- 百万位 π 计算涉及大整数除法与开方，耗时以秒~分钟计；示例程序中的 `calculatePi_Chudnovsky(1000000)` 属于压测演示。

## 📄 许可

暂未指定。如需开源，建议添加 MIT 或 Apache-2.0 许可证。
