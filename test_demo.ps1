Write-Host "=== BoltDB Demo Script ===" -ForegroundColor Green
Write-Host ""
Write-Host "This script will:" -ForegroundColor Yellow
Write-Host "1. Start the BoltDB server" -ForegroundColor Yellow
Write-Host "2. Wait for it to initialize" -ForegroundColor Yellow
Write-Host "3. Run the test client" -ForegroundColor Yellow
Write-Host "4. Stop the server" -ForegroundColor Yellow
Write-Host ""
Read-Host "Press Enter to continue"

Write-Host "Starting BoltDB server..." -ForegroundColor Cyan
$serverProcess = Start-Process -FilePath ".\boltdb.exe" -WindowStyle Minimized -PassThru
Write-Host "Waiting for server to initialize..." -ForegroundColor Cyan
Start-Sleep -Seconds 3

Write-Host "Running test client..." -ForegroundColor Cyan
.\simple_test_client.exe

Write-Host ""
Write-Host "Test completed! Stopping server..." -ForegroundColor Cyan
Stop-Process -Id $serverProcess.Id -Force -ErrorAction SilentlyContinue
Write-Host "Server stopped." -ForegroundColor Green
Write-Host ""
Read-Host "Press Enter to exit"


