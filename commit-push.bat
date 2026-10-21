@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion
cd /d "%~dp0"
title Mathmatic_Function 一键提交

REM ============ 1. 暂存全部改动 ============
git add -A
git diff --cached --quiet
if %errorlevel%==0 goto :push

REM ============ 2. 生成提交信息并提交 ============
set "MSG=%~1"
if "%MSG%"=="" (
    for /f %%i in ('powershell -NoProfile -Command "Get-Date -Format yyyy-MM-dd_HH:mm"') do set "MSG=update: %%i"
)
echo [提交] !MSG!
git commit -m "!MSG!"
if errorlevel 1 goto :fail_commit

:push
REM ============ 3. 无论是否有新提交，都推送到 GitHub（兜住上次推送失败的情况） ============
echo [推送] git push origin master ...
git push origin master
if errorlevel 1 goto :fail_push

echo.
echo [完成] 本地与 GitHub 已同步。
git log --oneline -3
pause
exit /b 0

:fail_commit
echo [失败] 提交出错，请查看上方信息。
pause
exit /b 1

:fail_push
echo [失败] 推送失败，请检查网络，或先执行 git pull 同步远程改动。
pause
exit /b 1
