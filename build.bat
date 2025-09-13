@echo off
echo Building BoltDB...

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
cmake .. -G "Visual Studio 16 2019" -A x64

REM Build the project
cmake --build . --config Release

echo.
echo Build complete! Executable is in build\bin\Release\boltdb.exe
echo.
echo To run the server:
echo   build\bin\Release\boltdb.exe
echo.
echo To run the test client:
echo   g++ -std=c++17 -pthread -o test_client ..\test_client.cpp
echo   test_client.exe
