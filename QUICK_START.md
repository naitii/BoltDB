# BoltDB Quick Start Guide

## 🚀 **Fastest Way to Test BoltDB**

### **Option 1: Automated Demo (Recommended)**
```powershell
.\test_demo.bat
```
This will automatically start the server, run tests, and stop the server.

### **Option 2: PowerShell Demo**
```powershell
.\test_demo.ps1
```
Same as above but with colored output.

## 🎮 **Manual Testing**

### **Start Server**
```powershell
.\start_server.bat
```
Or directly:
```powershell
.\boltdb.exe
```

### **Stop Server**
```powershell
.\stop_server.bat
```
Or use Ctrl+C in the server window.

### **Test with Client**
```powershell
# Fast automated test
.\simple_test_client.exe

# Interactive testing
.\interactive_client.exe

# Manual testing with telnet
telnet localhost 7379
```

## 📋 **Available Commands**

- `SET key value` - Store a key-value pair
- `GET key` - Retrieve a value by key  
- `DELETE key` - Delete a key-value pair
- `QUIT` - Disconnect from server

## 🔧 **Troubleshooting**

**Server won't start?**
- Check if port 7379 is already in use
- Make sure you're in the correct directory

**Client can't connect?**
- Make sure the server is running first
- Check if the server is listening on port 7379

**Process won't stop?**
- Use `.\stop_server.bat` for reliable stopping
- Or use `taskkill /F /IM boltdb.exe`

## 📁 **Files**

- `boltdb.exe` - Main database server
- `simple_test_client.exe` - Fast automated test
- `interactive_client.exe` - Interactive testing
- `test_demo.bat` - Complete demo script
- `test_demo.ps1` - PowerShell demo script
- `start_server.bat` - Start server script
- `stop_server.bat` - Stop server script
- `dump.bdb` - Database persistence file (auto-created)


