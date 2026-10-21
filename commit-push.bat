@echo off
chcp 65001 >nul
cd /d "%~dp0"
title Mathmatic_Function 一键提交

REM ============ 1. 暂存全部改动 ============
git add -A
git diff --cached --quiet
if %errorlevel%==0 (
    echo [跳过] 没有需要提交的改动，工作区是干净的。
    pause
    exit /b 0
)

REM ============ 2. 生成提交信息 ============
set "MSG=%~1"
if "%MSG%"=="" (
    for /f %%i in ('powershell -NoProfile -Command "Get-Date -Format yyyy-MM-dd_HH:mm"') do set "MSG=update: %%i"
)

REM ============ 3. 提交 ============
echo [提交] %MSG%
git commit -m "%MSG%"
if errorlevel 1 (
    echo [失败] 提交出错，请查看上方信息。
    pause
    exit /b 1
)

REM ============ 4. 推送到 GitHub ============
echo [推送] git push origin master ...
git push origin master
if errorlevel 1 (
    echo [失败] 推送失败，请检查网络或先执行 git pull 同步远程改动。
    pause
    exit /b 1
)

echo.
echo [完成] 已提交并推送到 GitHub。
git log --oneline -3
pause
