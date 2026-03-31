@echo off
setlocal enabledelayedexpansion

:: 切换到脚本所在目录
cd /d "%~dp0"

:: 请求管理员权限
fltmc >nul 2>&1 || (
    echo 请求管理员权限...
    powershell start -verb runas '%0' %*
    exit /b
)

cd /d "%~dp0"

set "source_dir=MDK-ARM\.vscode"
set "target_dir=.vscode"

if not exist "%source_dir%\" (
    echo 错误：源目录 %source_dir% 不存在
    echo 请确保脚本位于项目根目录（包含 MDK-ARM 文件夹的上级目录）
    pause
    exit /b
)

if not exist "%target_dir%\" (
    echo 正在创建目标目录: %target_dir%
    mkdir "%target_dir%"
) else (
    echo 目标目录 %target_dir% 已存在
)

for /R "%source_dir%" %%F in (*) do (
    set "target_file=.vscode\%%~nxF"

    if exist "!target_file!" (
        echo 发现已存在的硬链接: %%~nxF，正在删除...
        del "!target_file!"
    )

    echo 正在创建硬链接: %%~nxF
    mklink /H "!target_file!" "%%F"
)

echo 操作完成
pause
