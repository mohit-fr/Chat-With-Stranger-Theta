#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <thread>
#include <chrono>
#include <atomic>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9909
using namespace std;
atomic<bool> running(true);

SOCKET nClientSocket;
struct sockaddr_in srv;

// Function to set console text color
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// Function to reset console text color to default
void resetColor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7); // 7 is default white color
}

void takeInput() {
    while (running) {
        char buff[256] = {0};
        if (fgets(buff, sizeof(buff), stdin) == nullptr) {
            setColor(12); // Red color for error
            cerr << "Failed to read input.\n";
            resetColor();
            break;
        }

        // Remove newline character from fgets
        buff[strcspn(buff, "\n")] = '\0';

        if (strcmp(buff, "exit") == 0) {
            setColor(12); // Red color for exit message
            cout << "Exiting...\n";
            resetColor();
            break;
        }

        int nRet = send(nClientSocket, buff, strlen(buff), 0);
        if (nRet == SOCKET_ERROR) {
            setColor(12); // Red color for error
            cerr << "Failed to send message to server.\n";
            resetColor();
            break;
        }
    }
}

void showOutput() {
    while (running) {
        char buff[256] = {0};
        int nRet = recv(nClientSocket, buff, sizeof(buff) - 1, 0);
        if (nRet > 0) {
            if (strlen(buff) > 0) {
                // Check if it's a system message
                if (strstr(buff, "Waiting for a partner") || 
                    strstr(buff, "connected to a random client") ||
                    strstr(buff, "disconnected")) {
                    setColor(14); // Yellow color for system messages
                } else {
                    setColor(10); // Green color for chat messages
                }
                cout << "Message From your friend : " << buff << "\n";
                resetColor();
            }
        } else if (nRet == 0) {
            setColor(12); // Red color for disconnection
            cout << "Server closed the connection.\n";
            resetColor();
            break;
        } else if (nRet == SOCKET_ERROR) {
            setColor(12); // Red color for error
            cerr << "No response from server (timed out).\n";
            resetColor();
        }
    }
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (nRet != 0) {
        setColor(12); // Red color for error
        cerr << "WSAStartup failed\n";
        resetColor();
        return 1;
    }

    nClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (nClientSocket == INVALID_SOCKET) {
        setColor(12); // Red color for error
        cerr << "Socket creation failed.\n";
        resetColor();
        WSACleanup();
        return 1;
    } else {
        setColor(11); // Cyan color for success
        cout << "Socket created successfully.\n";
        resetColor();
    }

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&srv.sin_zero, 0, 8);

    nRet = connect(nClientSocket, (struct sockaddr*)&srv, sizeof(srv));

    if (nRet == SOCKET_ERROR) {
        setColor(12); // Red color for error
        cerr << "Connection to server failed.\n";
        resetColor();
        closesocket(nClientSocket);
        WSACleanup();
        return 1;
    } else {
        setColor(11); // Cyan color for success
        cout << "Connected to server successfully.\n";
        resetColor();
        
        char buff[256] = {0};
        nRet = recv(nClientSocket, buff, sizeof(buff) - 1, 0);
        if (nRet > 0) {
            setColor(14); // Yellow color for system message
            cout << "Message from server: " << buff << "\n";
            resetColor();
        } else if (nRet == 0) {
            setColor(12); // Red color for disconnection
            cout << "Server closed the connection.\n";
            resetColor();
            closesocket(nClientSocket);
            WSACleanup();
            return 0;
        } else {
            setColor(12); // Red color for error
            cerr << "Failed to receive initial message from server.\n";
            resetColor();
            closesocket(nClientSocket);
            WSACleanup();
            return 1;
        }

        setColor(11); // Cyan color for instructions
        cout << "You can start sending messages to the server. Type 'exit' to quit.\n";
        resetColor();

        thread inputThread(takeInput);
        thread outputThread(showOutput);

        inputThread.join();
        outputThread.join();
    }

    closesocket(nClientSocket);
    WSACleanup();
    return 0;
}
