##############################################################################
#                    GCC/Windows Makefile for C++ Projects                   #
##############################################################################

# 定义目标文件与源文件
SRCS := $(wildcard *.cpp)  # 搜索所有 .cpp 文件
OBJS := $(patsubst %.cpp,%.o,$(SRCS))  # 将 .cpp 转换为 .o

# 定义编译器
CC := g++

# 定义编译选项
# -I 指定 Boost 头文件路径 (Chudnovsky 级数任意精度计算需要 boost/multiprecision)
CFLAGS := -Wall -g -I"D:/Program Files/MinGW64/include"

# 定义程序名
TARGET := Main

# 定义可执行文件
EXECUTABLE := $(TARGET).exe

# 默认目标
all: $(EXECUTABLE)

# 编译程序
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# 编译源文件为对象文件
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# 清理所有生成的文件
clean:
	cmd /C del /F /Q $(OBJS) $(EXECUTABLE)

# 运行程序（带星期几）
auto:
	@powershell -Command "Write-Host -ForegroundColor Yellow ('+--Started at: ' + (Get-Date -Format 'yyyy/MM/dd dddd HH:mm:ss'))"
	./$(EXECUTABLE)
	@powershell -Command "Write-Host -ForegroundColor Yellow ('+--Finished at: ' + (Get-Date -Format 'yyyy/MM/dd dddd HH:mm:ss'))"

# 进阶运行（带星期几）
run: clean $(EXECUTABLE)
	@powershell -Command "Write-Host -ForegroundColor Yellow ('+--Started at: ' + (Get-Date -Format 'yyyy/MM/dd dddd HH:mm:ss'))"
	./$(EXECUTABLE)
	@powershell -Command "Write-Host -ForegroundColor Yellow ('+--Finished at: ' + (Get-Date -Format 'yyyy/MM/dd dddd HH:mm:ss'))"

# 显示信息
show:
	@powershell -Command "Write-Host 'Sources: $(SRCS)' -ForegroundColor White"
	@powershell -Command "Write-Host 'Objects: $(OBJS)' -ForegroundColor White"
	@powershell -Command "Write-Host 'Compiler: $(CC)' -ForegroundColor White"
	@powershell -Command "Write-Host 'Flags: $(CFLAGS)' -ForegroundColor White"
	@powershell -Command "Write-Host 'Target: $(TARGET)' -ForegroundColor White"
	@powershell -Command "Write-Host 'Executable: $(EXECUTABLE)' -ForegroundColor White"

# 测试颜色
test-color:
	@powershell -Command "Write-Host 'This is green' -ForegroundColor Green"
	@powershell -Command "Write-Host 'This is red' -ForegroundColor Red"

# 声明.PHONY目标
.PHONY: all clean run show test-color auto
