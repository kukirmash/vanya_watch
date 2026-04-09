@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

set folders="0-1" "1-2" "2-3" "3-4" "4-5" "5-6" "6-7" "7-8" "8-9" "9-0" "2-0" "3-0" "5-0"

for %%f in (%folders%) do (
    echo Конвертирую папку: %%~f в формат A1 ^(1-bit маска^)...
    py LVGLImage.py --ofmt C --cf A1 --compress NONE "%%~f"
)

echo.
echo Конвертация завершена!
pause