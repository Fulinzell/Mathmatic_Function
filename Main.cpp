#include "Head.h"

int main()
{
    /*  案例一：--------------------------------------------------------------------------二维点-------------------------------------------------------------------------------*/

    // 创建一个二维点的实例
    POINT_2D Point2D_P1;

    Point2D_P1.X_Pos = 10.0;
    Point2D_P1.Y_Pos = 20.0;
    Point2D_P1.K_Val = 0.0; // 斜率值

    // 创建另一个二维点的实例
    POINT_2D Point2D_P2;
    Point2D_P2.X_Pos = 30.0;
    Point2D_P2.Y_Pos = 80.0;
    Point2D_P2.K_Val = 0.0; // 斜率值

    // 输出二维点的坐标和斜率
    std::cout << std::fixed << std::setprecision(4); // 设置浮点数显示为小数点后4位
    std::cout << "Point_2D Coordinates: (" << Point2D_P1.X_Pos << ", " << Point2D_P1.Y_Pos << ")," << "K_Value:" << calculateSlope(Point2D_P1, Point2D_P2) << endl
              << endl;

    /*  --------------------------------------------------------------------------三阶方阵-------------------------------------------------------------------------------*/
    // 创建一个三维矩阵实例
    MATRIX_3D Martix = {
        {{1.0, 2.0, 3.0},
         {4.0, 5.0, 6.0},
         {7.0, 8.0, 9.0}}};

    // 计算矩阵的行列式
    double determinant = calculate_Determinant(Martix);

    // 输出行列式的值
    std::cout << "Det(";
    print_Matrix(Martix);

    std::cout << ") = " << determinant << std::endl
              << endl;

    /* 案例三 *****************************************************************三维点********************************************************************** */
    // 创建一个三维点的实例

    // 计算三维点之间的斜率

    try
    {
        POINT_3D Point_3D_1 = {1.0, 2.0, 3.0, 0.0};
        POINT_3D Point_3D_2 = {4.0, 5.0, 6.0, 0.0};

        double cos_Alpha, cos_Beta, cos_Gamma;
        calculateDirectionCosines(Point_3D_1, Point_3D_2, cos_Alpha, cos_Beta, cos_Gamma);

        std::cout << "Direction Cosines:" << std::endl;
        std::cout << "Cos_Alpha (X): " << cos_Alpha << std::endl;
        std::cout << "Cos_Beta  (Y): " << cos_Beta << std::endl;
        std::cout << "Cos_Gamma (Z): " << cos_Gamma << std::endl;
    }
    catch (const std::invalid_argument &warn)
    {
        std::cerr << "Error: " << warn.what() << std::endl;
    }

    /* 案例四：----------------------------------------------阶乘 / 伽马函数 / 斯特林公式------------------------------------------------------*/

    // 任意数阶乘（返回十进制字符串，无上限）
    std::cout << "Factorial(10)    = " << Factorial(10) << std::endl;
    std::cout << "Factorial(25)    = " << Factorial(25) << std::endl;
    std::cout << "Factorial(100)   = " << Factorial(100) << std::endl;

    // 伽马函数（支持小数阶乘：x! = Γ(x+1)）
    std::cout << "Gamma(0.5)       = " << Gamma_Function(0.5) << std::endl;  // √π ≈ 1.77245
    std::cout << "Gamma(5.5)       = " << Gamma_Function(5.5) << std::endl;  // 4.5! ≈ 52.34278
    std::cout << "Gamma(-0.5)      = " << Gamma_Function(-0.5) << std::endl; // −2√π ≈ −3.54491

    // 斯特林公式（近似值与对数形式）
    std::cout << "Stirling(10)     = " << Stirling_Approximation(10) << std::endl; // ≈ 10! = 3628800
    std::cout << "StirlingLn(100)  = " << Stirling_Ln(100) << std::endl;           // ≈ ln(100!) ≈ 363.73938
    std::cout << "Stirling(100)  = " << Stirling_Approximation(100) << std::endl;  // ≈ 100! = 9.3326215443944154e+15

    /* 案例五：----------------------------------------------Chudnovsky 级数计算圆周率------------------------------------------------------*/

    // 双精度版：观察每加一项增加的正确的十进制位数（每项约 +14 位）
    std::cout << std::setprecision(17);
    for (unsigned int terms = 1; terms <= 3; ++terms)
    {
        std::cout << "Chudnovsky_Pi(terms = " << terms << ") = "
                  << Chudnovsky_Pi(terms) << std::endl;
    }

    // 任意精度版：100 位有效数字（项数自动选取）
    std::cout << "Chudnovsky_Pi(100 digits) =" << std::endl
              << Chudnovsky_Pi(100, 0) << std::endl;

    // 拉马努金级数（double, 每项约 +8 位正确数字, Pi.cpp 实现）
    std::cout << "Ramanujan(1)     = " << calculatePi_Ramanujan(1) << std::endl;
    std::cout << "Ramanujan(2)     = " << calculatePi_Ramanujan(2) << std::endl;
    std::cout << "Ramanujan(3)     = " << calculatePi_Ramanujan(3) << std::endl;

    // 整数算术版 Chudnovsky（Pi.cpp 实现, 任意位数, 全程整数运算, 每一位都是真值）
    std::cout << "Chudnovsky_Int(100) = " << calculatePi_Chudnovsky(100) << std::endl;
    std::cout << "Chudnovsky_Int(200) = " << calculatePi_Chudnovsky(200) << std::endl;
    std::cout << "Chudnovsky_Int(500) = " << calculatePi_Chudnovsky(500) << std::endl;
    std::cout << "Chudnovsky_Int(1000000) = " << calculatePi_Chudnovsky(1000000) << std::endl;

    // 上述每个 π 函数执行时都会把结果写入当前目录的 *_Bits.md 文件（可用 setPiOutputDirectory 指定输出目录）
    std::cout << "[Info] 各 π 计算结果已写入当前目录下的 *_Bits.md 文件 (setPiOutputDirectory 可指定目录)" << std::endl;

    return 0;
}