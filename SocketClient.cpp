#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9909
using namespace std;
atomic<bool> running(true);

SOCKET nClientSocket;
struct sockaddr_in srv;
string userName;
bool nameSet = false;

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

        // Don't echo the input since the server will send it back
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
        int color;
        int nRet = recv(nClientSocket, (char*)&color, sizeof(int), 0);
        if (nRet <= 0) {
            if (nRet == 0) {
                setColor(12); // Red color for disconnection
                cout << "\nServer closed the connection.\n";
                resetColor();
            } else {
                setColor(12); // Red color for error
                cerr << "\nNo response from server (timed out).\n";
                resetColor();
            }
            break;
        }

        char buff[256] = {0};
        nRet = recv(nClientSocket, buff, sizeof(buff) - 1, 0);
        if (nRet > 0) {
            if (strlen(buff) > 0) {
                string message(buff);
                
                // Handle different types of messages with different colors
                if (message.find("Please enter your name:") != string::npos) {
                    setColor(11); // Cyan for prompts
                    cout << message;
                    resetColor();
                }
                else if (message.find("Welcome to Theta Chat!") != string::npos) {
                    setColor(11); // Cyan for welcome message (same as socket created)
                    cout << message << "\n\n";  // Add extra line gap
                    resetColor();
                }
                else if (message.find(" has joined the chat!") != string::npos ||
                         message.find(" has left the chat.") != string::npos) {
                    setColor(14); // Yellow for system messages
                    cout << message << "\n";
                    resetColor();
                }
                else {
                    // Regular chat message
                    setColor(color); // Use received color for chat messages
                    cout << message << "\n";
                    resetColor();
                }
            }
        }

        if (strstr(buff, "/clear") != NULL) {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            COORD coordScreen = {0, 0};
            DWORD cCharsWritten;
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            DWORD dwConSize;

            // Get the number of character cells in the current buffer
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

            // Fill the entire screen with blanks
            FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);

            // Get the current text attribute
            GetConsoleScreenBufferInfo(hConsole, &csbi);

            // Set the buffer's attributes accordingly
            FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);

            // Put the cursor at its home coordinates
            SetConsoleCursorPosition(hConsole, coordScreen);

            // Now show the cleared message
            SetConsoleTextAttribute(hConsole, 10); // Green
            cout << "Chat screen cleared.\n";
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
        cout << "Socket created successfully.\n\n";  // Add line gap
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
        cout << "Connected to server successfully.\n\n";  // Add line gap
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
