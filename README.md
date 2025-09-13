# BoltDB - In-Memory Key-Value Database

A simple, high-performance, concurrent in-memory key-value database built in C++17. BoltDB accepts client connections over TCP and provides thread-safe operations with automatic persistence.

## Features

- **Thread-Safe Operations**: Uses `std::mutex` to protect concurrent access to the data store
- **Multi-Threaded TCP Server**: Each client connection is handled in a dedicated thread
- **Simple Command Protocol**: Text-based protocol with SET, GET, DELETE commands
- **Automatic Persistence**: Background thread saves data to disk every 60 seconds
- **Cross-Platform**: Works on Windows and Unix-like systems
- **Modern C++**: Built with C++17 features including smart pointers and threading

## Architecture

### Core Components

1. **DataStore**: Thread-safe in-memory hash map (`std::unordered_map<std::string, std::string>`)
2. **PersistenceManager**: Handles saving/loading data to/from disk
3. **Server**: Multi-threaded TCP server with client connection handling
4. **Command Protocol**: Simple text-based protocol for client communication

### Command Protocol

All commands are terminated by a newline character (`\n`):

- `SET key value` - Store a key-value pair
  - Response: `+OK\n` (success) or `-ERR message\n` (error)
- `GET key` - Retrieve a value by key
  - Response: `$length\nvalue\n` (found) or `$-1\n` (not found)
- `DELETE key` - Delete a key-value pair
  - Response: `:1\n` (deleted) or `:0\n` (not found)
- `QUIT` - Disconnect from server
  - Response: `+OK\n`

## Building

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10 or later

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build . --config Release

# On Windows, the executable will be in build/bin/Release/
# On Unix-like systems, the executable will be in build/bin/
```

### Alternative Build (without CMake)

```bash
# Compile directly (adjust for your compiler)
g++ -std=c++17 -O2 -pthread -o boltdb main.cpp datastore.cpp persistence.cpp server.cpp

# On Windows with MSVC, you may need to link ws2_32.lib
```

## Usage

### Starting the Server

```bash
# Start with default port (7379) and dump file (dump.bdb)
./boltdb

# Start with custom port
./boltdb 8080

# Start with custom port and dump file
./boltdb 8080 mydata.bdb

# Show help
./boltdb --help
```

### Testing with the Test Client

```bash
# Build the test client
g++ -std=c++17 -pthread -o test_client test_client.cpp

# Run the test client (make sure server is running)
./test_client
```

### Manual Testing with telnet

```bash
# Connect to the server
telnet localhost 7379

# Test commands
SET name Alice
GET name
DELETE name
QUIT
```

## Example Session

```
$ ./boltdb
=== BoltDB - In-Memory Key-Value Database ===
Version: 1.0.0
Author: C++ Implementation

Data store initialized
Loaded 0 entries from dump.bdb
Persistence manager started
Starting server on port 7379...
Database file: dump.bdb
Press Ctrl+C to stop the server

Client 1 connected
Client 1 disconnected
```

## File Format

The persistence file (`dump.bdb` by default) uses a simple CSV format:
```
key1,value1
key2,value2
key with spaces,value with spaces
```

Special characters are escaped:
- Commas are escaped as `\c`
- Newlines are escaped as `\n`

## Thread Safety

- All data store operations are protected by a single mutex
- Each client connection runs in its own thread
- Persistence operations are thread-safe and don't block client operations
- The server can handle multiple concurrent clients safely

## Error Handling

- Network errors are logged and clients are disconnected gracefully
- File I/O errors are logged but don't crash the server
- Invalid commands return error responses
- Signal handling allows graceful shutdown (Ctrl+C)

## Performance Considerations

- Uses a single mutex for all data operations (simple but may limit concurrency)
- Persistence happens in background thread every 60 seconds
- No connection pooling or advanced networking optimizations
- Suitable for moderate load applications

## Limitations

- In-memory only (data lost if server crashes between saves)
- Single mutex may limit high-concurrency scenarios
- No authentication or authorization
- No replication or clustering
- Simple text protocol (not optimized for high throughput)

## License

This is a demonstration project. Use at your own discretion.

## Contributing

This is a learning project. Feel free to fork and experiment with improvements!
