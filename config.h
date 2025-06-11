#ifndef CONFIG_H
#define CONFIG_H

// Network Configuration
#define DEFAULT_PORT 9909
#define DEFAULT_IP "127.0.0.1"
#define MAX_CLIENTS 5
#define BUFFER_SIZE 256

// Timeout Configuration (in milliseconds)
#define CONNECTION_TIMEOUT 5000
#define SEND_TIMEOUT 1000
#define RECV_TIMEOUT 1000

// Error Messages
#define ERR_SOCKET_CREATE "Failed to create socket"
#define ERR_SOCKET_BIND "Failed to bind socket"
#define ERR_SOCKET_LISTEN "Failed to listen on socket"
#define ERR_SOCKET_ACCEPT "Failed to accept connection"
#define ERR_SOCKET_CONNECT "Failed to connect to server"
#define ERR_SOCKET_SEND "Failed to send data"
#define ERR_SOCKET_RECV "Failed to receive data"

// Success Messages
#define MSG_SOCKET_CREATE "Socket created successfully"
#define MSG_SOCKET_BIND "Socket bound successfully"
#define MSG_SOCKET_LISTEN "Socket listening successfully"
#define MSG_SOCKET_CONNECT "Connected to server successfully"
#define MSG_WAITING "Waiting for a partner..."
#define MSG_CONNECTED "You are now connected to a random client"
#define MSG_DISCONNECTED "Your chat partner disconnected. Searching for a new partner..."

#endif // CONFIG_H 