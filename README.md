# Theta - Multi-User Chat

A real-time chat application built with C++ and socket programming. Connect with multiple users in a shared space with colored messages and user names.

## Features

- Real-time multi-user chat
- Colored messages for different users
- User names and system notifications
- Commands: /list, /clear, /time, /count, /help, /msg <Username>

## Build & Run

### Windows
```bash
g++ -o server.exe server.cpp -lws2_32
g++ -o client.exe SocketClient.cpp -lws2_32
```

### Linux/macOS
```bash
g++ -o server server.cpp
g++ -o client SocketClient.cpp
```

## Usage

1. Start server: `./server` or `server.exe`
2. Start clients: `./client` or `client.exe`
3. Enter your name when prompted
4. Start chatting!
5. Type 'exit' to quit

## License

MIT License - Copyright (c) 2024 Mohit Budhlakoti 