# Theta - Multi-User Chat Application

A real-time multi-user chat application built using C++ and socket programming. It allows multiple users to connect and chat in a shared space with colored messages and user names.

## Features

- Real-time multi-user chat
- User names for message identification
- Colored messages for different users
- System notifications for user join/leave
- No message storage (ephemeral chat)
- Cross-platform support (Windows, Linux, macOS)

## Prerequisites

- C++11 or later
- Windows: MinGW-w64 or Visual Studio
- Linux/macOS: GCC or Clang

## Building the Project

### Windows

1. Install MinGW-w64 if you haven't already
2. Add MinGW's bin directory to your system's PATH
3. Compile the server:
   ```bash
   g++ -o server.exe server.cpp -lws2_32
   ```
4. Compile the client:
   ```bash
   g++ -o client.exe SocketClient.cpp -lws2_32
   ```

### Linux/macOS

1. Compile the server:
   ```bash
   g++ -o server server.cpp
   ```
2. Compile the client:
   ```bash
   g++ -o client SocketClient.cpp
   ```

## Running the Application

1. Start the server:
   ```bash
   # Windows
   .\server.exe
   
   # Linux/macOS
   ./server
   ```

2. Start one or more clients:
   ```bash
   # Windows
   .\client.exe
   
   # Linux/macOS
   ./client
   ```

## Usage

1. Start the server first
2. Start multiple client instances
3. When a client connects, they will be prompted to enter their name
4. After entering the name, they can start chatting
5. Messages will be displayed with the sender's name
6. Each user's messages will have a different color
7. Type 'exit' to quit a client

## Configuration

The following parameters can be modified in the source code:
- `PORT`: The port number for the server (default: 9909)
- Maximum number of concurrent clients (default: 5)
- Message colors for different users

## Security

- No message storage
- Anonymous communication
- No user authentication required
- Messages are lost when clients disconnect

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

Mohit Budhlakoti - w.moheat@gmail.com

## Acknowledgments

- Socket Programming
- TCP/IP Protocol
- C++ Standard Library 