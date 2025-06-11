#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <queue>
#include <map>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9909
using namespace std;

struct sockaddr_in srv;
fd_set fr, fw, fe;
int nMaxFd;
SOCKET nSocket;

queue<SOCKET> availableClients; // Queue to manage available clients
map<SOCKET, SOCKET> clientPairs;   // Map to manage client pairs

void ProcessNewMessage(SOCKET nClientSocket) {
    cout << "\nProcessing the new message for client socket: " << nClientSocket << endl;
    char buff[256 + 1] = {0,};
    int nRet = recv(nClientSocket, buff, 256, 0);

    if (nRet < 0) {
        cout << "\nError occurred, closing connection for client: " << nClientSocket << endl;
        closesocket(nClientSocket);
        
        SOCKET pairedClient = clientPairs[nClientSocket];
        clientPairs.erase(nClientSocket);
        clientPairs.erase(pairedClient);

        if (pairedClient != INVALID_SOCKET) {
            send(pairedClient, "Your chat partner disconnected. Searching for a new partner...", 61, 0);
            availableClients.push(pairedClient);
        }

    } else {
        SOCKET pairedClient = clientPairs[nClientSocket];
        if (pairedClient != INVALID_SOCKET) {
            send(pairedClient, buff, sizeof(buff), 0);
        }
        cout << "\n******************************************************************\n";
    }
}

void ProcessTheNewRequest() {
    if (FD_ISSET(nSocket, &fr)) {
        struct sockaddr_in clientAddr;
        int nLen = sizeof(clientAddr);
        SOCKET nClientSocket = accept(nSocket, (struct sockaddr *)&clientAddr, &nLen);

        if (nClientSocket != INVALID_SOCKET) {
            if (availableClients.empty()) {
                availableClients.push(nClientSocket);
                send(nClientSocket, "Waiting for a partner...", 26, 0);
            } else {
                SOCKET pairedClient = availableClients.front();
                availableClients.pop();

                clientPairs[nClientSocket] = pairedClient;
                clientPairs[pairedClient] = nClientSocket;

                send(nClientSocket, "You are now connected to a random client", 41, 0);
                send(pairedClient, "You are now connected to a random client", 41, 0);
            }

            if (nClientSocket > nMaxFd) {
                nMaxFd = nClientSocket;
            }
        } else {
            cout << "\nError accepting connection\n";
        }
    } else {
        for (auto it = clientPairs.begin(); it != clientPairs.end(); ++it) {
            SOCKET clientSocket = it->first;
            if (FD_ISSET(clientSocket, &fr)) {
                ProcessNewMessage(clientSocket);
            }
        }
    }
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (nRet != 0) {
        cout << "WSAStartup failed\n";
        return 1;
    }

    nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (nSocket == INVALID_SOCKET) {
        cout << "\nSocket could not be opened\n";
        WSACleanup();
        return 1;
    } else {
        cout << "\nSocket opened successfully\n";
    }

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = INADDR_ANY;
    memset(&(srv.sin_zero), 0, 8);

    int optVal = 1;
    nRet = setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, sizeof(optVal));

    if (nRet == 0) {
        cout << "\nSetsockopt call successful\n";
    } else {
        cout << "\nFailed\n";
        closesocket(nSocket);
        WSACleanup();
        return 1;
    }

    nRet = bind(nSocket, (sockaddr*)&srv, sizeof(sockaddr));

    if (nRet == SOCKET_ERROR) {
        cout << "\nFailed to bind to the local port\n";
        closesocket(nSocket);
        WSACleanup();
        return 1;
    } else {
        cout << "\nSuccessfully bound to local port\n";
    }

    nRet = listen(nSocket, 5);
    if (nRet == SOCKET_ERROR) {
        cout << "\nFailed to listen to the local port\n";
        closesocket(nSocket);
        WSACleanup();
        return 1;
    } else {
        cout << "\nStarted listening to the local port\n";
    }

    nMaxFd = nSocket;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while (true) {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);

        FD_SET(nSocket, &fr);
        FD_SET(nSocket, &fe);

        for (auto it = clientPairs.begin(); it != clientPairs.end(); ++it) {
            SOCKET clientSocket = it->first;
            if (clientSocket != INVALID_SOCKET) {
                FD_SET(clientSocket, &fr);
                FD_SET(clientSocket, &fe);
            }
        }

        nRet = select(nMaxFd + 1, &fr, &fw, &fe, &tv);
        if (nRet > 0) {
            ProcessTheNewRequest();
        } else if (nRet == 0) {
            // No connection or any other request
        } else {
            cout << "Nothing in PORT: " << PORT << endl;
        }
    }

    // Cleanup
    closesocket(nSocket);
    WSACleanup();
    return 0;
}
