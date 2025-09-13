@echo off
echo === BoltDB Demo Script ===
echo.
echo This script will:
echo 1. Start the BoltDB server
echo 2. Wait for it to initialize
echo 3. Run the test client
echo 4. Stop the server
echo.
pause

echo Starting BoltDB server...
start /B .\boltdb.exe
echo Waiting for server to initialize...
timeout /t 3 /nobreak >nul

echo Running test client...
.\simple_test_client.exe

echo.
echo Test completed! Stopping server...
taskkill /F /IM boltdb.exe 2>nul
echo Server stopped.
echo.
pause


