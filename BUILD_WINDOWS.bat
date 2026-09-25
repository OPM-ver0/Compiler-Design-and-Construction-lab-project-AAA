@echo off
setlocal

echo Building Sadhu Bangla Compiler with Flex...
if exist lex.yy.cpp del /q lex.yy.cpp
if exist sadhu_bangla_compiler.exe del /q sadhu_bangla_compiler.exe
flex -o lex.yy.cpp sadhu_bangla_lexer.l
if errorlevel 1 (
    echo.
    echo Flex build failed. Make sure Flex is installed and available on PATH.
    exit /b 1
)

g++ -std=c++17 -O2 -Wall -Wextra -pedantic lex.yy.cpp sadhu_bangla_flex_main.cpp -o sadhu_bangla_compiler.exe
if errorlevel 1 (
    echo.
    echo C++ build failed. Make sure MinGW-w64 g++ is installed and available on PATH.
    exit /b 1
)

echo.
echo Build successful: sadhu_bangla_compiler.exe
endlocal
