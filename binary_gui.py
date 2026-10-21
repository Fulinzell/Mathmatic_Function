# -*- coding: utf-8 -*-
"""
十进制 → 二进制 可视化转换器 (PyQt5)

与 C++ 库 Decimal_To_Binary 相同的字符串拼接算法（全程字符运算，无浮点误差）：
  - 整数部分：除 2 取余，逆序拼接
  - 小数部分：乘 2 取整（位数用尽未精确表示时结果带省略标记）

用法：python binary_gui.py
"""

import sys

from PyQt5.QtCore import Qt
from PyQt5.QtGui import QColor, QFont
from PyQt5.QtWidgets import (
    QApplication,
    QCheckBox,
    QGridLayout,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QMainWindow,
    QMessageBox,
    QPushButton,
    QScrollArea,
    QSpinBox,
    QVBoxLayout,
    QWidget,
)

MAX_DISPLAY_BITS = 512        # 格子可视化最多展示的位数（超出部分以文本显示）
CELL_SIZE = 30                # 每个格子的边长（像素）
BITS_PER_ROW = 24             # 每行格子数


# ---------------------------------------------------------------------------
# 核心算法：十进制字符串 → 二进制字符串（与 C++ 版 Decimal_To_Binary 对齐）
# ---------------------------------------------------------------------------
def decimal_to_binary(num: str, max_frac_bits: int = 52):
    """返回 (binary_str, truncated)。truncated 为 True 表示小数未精确表示。"""
    s = num.strip()
    if not s:
        raise ValueError("输入为空")
    negative = False
    if s[0] in "+-":
        negative = s[0] == "-"
        s = s[1:]
        if not s:
            raise ValueError("仅有符号，没有数字")
    if s.count(".") > 1:
        raise ValueError("小数点最多只能有一个")
    int_part, _, frac_part = s.partition(".")
    if not int_part:
        int_part = "0"
    if not frac_part:
        frac_part = "0"
    if not int_part.isdigit() or not frac_part.isdigit():
        raise ValueError("只能包含数字、小数点和负号")

    # 整数部分：除 2 取余
    n = int(int_part)  # Python 整数无上限，效果等同 C++ 的字符串除法
    int_bits = bin(n)[2:]

    # 小数部分：乘 2 取整（字符串运算，保持 10 的负幂次尺度，前导零不能丢）
    truncated = False
    frac_bits = ""
    if frac_part.strip("0"):
        frac = frac_part
        for _ in range(max_frac_bits):
            if not frac.strip("0"):
                break
            carry = 0
            digits = []
            for ch in reversed(frac):
                cur = (ord(ch) - 48) * 2 + carry
                digits.append(chr(48 + cur % 10))
                carry = cur // 10
            frac_bits += str(carry)          # 最高位进位即二进制位
            frac = "".join(reversed(digits))
        truncated = bool(frac.strip("0"))

    result = ("-" if negative else "") + int_bits
    if frac_bits:
        result += "." + frac_bits
    if truncated:
        result += "…1"  # 与 C++ 版一致：标记后续仍有非零位
    return result, truncated


# ---------------------------------------------------------------------------
# 主窗口
# ---------------------------------------------------------------------------
class BinaryWindow(QMainWindow):
    COLOR_INT_ONE = QColor("#2196F3")    # 整数部分 1：蓝
    COLOR_INT_ZERO = QColor("#BBDEFB")   # 整数部分 0：浅蓝
    COLOR_FRAC_ONE = QColor("#FF9800")   # 小数部分 1：橙
    COLOR_FRAC_ZERO = QColor("#FFE0B2")  # 小数部分 0：浅橙
    COLOR_DOT = QColor("#424242")

    def __init__(self):
        super().__init__()
        self.setWindowTitle("十进制 → 二进制 可视化转换器")
        self.resize(860, 480)

        central = QWidget()
        self.setCentralWidget(central)
        layout = QVBoxLayout(central)
        layout.setSpacing(10)

        # ---- 输入行 ----
        input_row = QHBoxLayout()
        input_row.addWidget(QLabel("十进制数："))
        self.input_edit = QLineEdit()
        self.input_edit.setPlaceholderText("输入整数或小数，如 25、-25、3.14159、0.0005")
        self.input_edit.returnPressed.connect(self.convert)
        input_row.addWidget(self.input_edit, stretch=1)

        input_row.addWidget(QLabel("小数精度："))
        self.precision_spin = QSpinBox()
        self.precision_spin.setRange(1, 1024)
        self.precision_spin.setValue(52)
        self.precision_spin.setSuffix(" 位")
        input_row.addWidget(self.precision_spin)

        self.convert_button = QPushButton("转换")
        self.convert_button.setFixedWidth(120)
        self.convert_button.clicked.connect(self.convert)
        input_row.addWidget(self.convert_button)

        self.cpp_check = QCheckBox("C++ 引擎 (Main.exe)")
        self.cpp_check.setToolTip("勾选后由 C++ 库的 Decimal_To_Binary 计算（通过 stdout 管道通信）")
        input_row.addWidget(self.cpp_check)
        layout.addLayout(input_row)

        # ---- 结果文本 ----
        layout.addWidget(QLabel("二进制结果："))
        self.result_edit = QLineEdit()
        self.result_edit.setReadOnly(True)
        self.result_edit.setFont(QFont("Consolas", 11))
        layout.addWidget(self.result_edit)

        # ---- 状态提示 ----
        self.status_label = QLabel("")
        self.status_label.setStyleSheet("color: #D32F2F;")
        layout.addWidget(self.status_label)

        # ---- 格子区（滚动） ----
        layout.addWidget(QLabel("位可视化（蓝 = 整数部分，橙 = 小数部分；深色 = 1，浅色 = 0）："))
        self.grid_widget = QWidget()
        self.grid_layout = QGridLayout(self.grid_widget)
        self.grid_layout.setSpacing(4)
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setWidget(self.grid_widget)
        layout.addWidget(scroll, stretch=1)

        # 图例
        legend = QHBoxLayout()
        for text, color in [
            ("整数位 1", self.COLOR_INT_ONE), ("整数位 0", self.COLOR_INT_ZERO),
            ("小数位 1", self.COLOR_FRAC_ONE), ("小数位 0", self.COLOR_FRAC_ZERO),
        ]:
            cell = QLabel()
            cell.setFixedSize(16, 16)
            cell.setStyleSheet(f"background-color: {color.name()}; border: 1px solid #888;")
            legend.addWidget(cell)
            legend.addWidget(QLabel(text))
        legend.addStretch(1)
        layout.addLayout(legend)

        self.input_edit.setFocus()

    # ------------------------------------------------------------------
    def convert_via_cpp(self, text: str, precision: int) -> str:
        """调用 C++ 引擎 Main.exe（stdout 管道：exe 打印 BINARY=..., Python 捕获解析）。"""
        import subprocess
        from pathlib import Path

        exe = Path(__file__).resolve().parent / "Main.exe"
        if not exe.exists():
            raise RuntimeError(f"未找到 C++ 引擎：{exe}\n请先执行 mingw32-make 编译 Main.exe")
        proc = subprocess.run(
            [str(exe), text, str(precision)],
            timeout=30,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            # Main.exe 输出 UTF-8 字节（含 "…" 省略标记），必须显式指定解码，
            # 否则 Windows 下按 GBK 解码会抛 UnicodeDecodeError
        )
        out = proc.stdout.decode("utf-8", errors="replace")
        err = proc.stderr.decode("utf-8", errors="replace")
        if proc.returncode != 0:
            detail = err.strip() or out.strip() or f"退出码 {proc.returncode}"
            raise RuntimeError(f"C++ 引擎报错：{detail}")
        for line in out.splitlines():
            if line.startswith("BINARY="):
                return line[len("BINARY="):]
        raise RuntimeError(f"C++ 引擎输出无法解析：{out.strip()!r}")

    # ------------------------------------------------------------------
    def convert(self):
        text = self.input_edit.text()
        try:
            if self.cpp_check.isChecked():
                # C++ 引擎：截断标记由结果末尾的 "…1" 判断
                binary = self.convert_via_cpp(text, self.precision_spin.value())
                truncated = binary.endswith("…1")
            else:
                binary, truncated = decimal_to_binary(text, self.precision_spin.value())
        except (ValueError, RuntimeError) as exc:
            QMessageBox.warning(self, "输入错误", str(exc))
            return

        self.result_edit.setText(binary)
        self.status_label.setText(
            "计算引擎：C++ (Main.exe)｜" if self.cpp_check.isChecked() else ""
        )
        if truncated:
            self.status_label.setText(
                self.status_label.text() + "注意：小数部分已按精度截断，未精确表示（末位之后仍有非零位）。"
            )
        self.fill_grid(binary, truncated)

    # ------------------------------------------------------------------
    def fill_grid(self, binary: str, truncated: bool):
        # 清空旧格子
        while self.grid_layout.count():
            item = self.grid_layout.takeAt(0)
            if item.widget() is not None:
                item.widget().deleteLater()

        negative = binary.startswith("-")
        body = binary.lstrip("-")
        body = body.rstrip("…1") if truncated else body  # 去掉省略标记
        int_part, _, frac_part = body.partition(".")

        bits = []  # (字符, 是否整数位, 权重说明)
        n_int = len(int_part)
        for i, ch in enumerate(int_part):
            bits.append((ch, True, f"2^{n_int - 1 - i}"))
        if frac_part:
            bits.append((".", None, "小数点"))
            for j, ch in enumerate(frac_part):
                bits.append((ch, False, f"2^-{j + 1}"))

        if negative:
            sign = QLabel("-")
            sign.setStyleSheet("color: #D32F2F; font-weight: bold; font-size: 16px;")
            self.grid_layout.addWidget(sign, 0, 0)

        display = bits[: MAX_DISPLAY_BITS + 1]
        offset = 1 if negative else 0
        for idx, (ch, is_int, weight) in enumerate(display):
            row, col = divmod(idx + offset, BITS_PER_ROW)
            if ch == ".":
                dot = QLabel("●")
                dot.setAlignment(Qt.AlignCenter)
                dot.setStyleSheet(f"color: {self.COLOR_DOT.name()}; font-size: 10px;")
                dot.setFixedSize(CELL_SIZE, CELL_SIZE)
                dot.setToolTip(weight)
                self.grid_layout.addWidget(dot, row, col)
                continue
            cell = QLabel(ch)
            cell.setAlignment(Qt.AlignCenter)
            cell.setFixedSize(CELL_SIZE, CELL_SIZE)
            if is_int:
                color = self.COLOR_INT_ONE if ch == "1" else self.COLOR_INT_ZERO
            else:
                color = self.COLOR_FRAC_ONE if ch == "1" else self.COLOR_FRAC_ZERO
            cell.setStyleSheet(
                f"background-color: {color.name()}; border: 1px solid #666; "
                f"color: {'white' if ch == '1' else '#555'}; font-weight: bold;"
            )
            cell.setToolTip(f"位权 {weight}")
            self.grid_layout.addWidget(cell, row, col)

        if len(bits) > len(display):
            row = (len(display) + offset) // BITS_PER_ROW
            more = QLabel(f"…… 其余 {len(bits) - len(display)} 位见上方文本结果")
            more.setStyleSheet("color: #888;")
            self.grid_layout.addWidget(more, row, 0, 1, BITS_PER_ROW)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = BinaryWindow()
    win.show()
    sys.exit(app.exec_())
