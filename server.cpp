#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <queue>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9909
using namespace std;

struct sockaddr_in srv;
fd_set fr, fw, fe;
int nMaxFd;
SOCKET nSocket;

struct ClientInfo 
{
    string name;
    int color;
};

map<SOCKET, ClientInfo> clients;
vector<SOCKET> activeClients;

const int COLORS[] = {9, 10, 11, 12, 13, 14};
int currentColorIndex = 0;

void sendPrivateMessage(SOCKET senderSocket, const string& recipientName, const string& message) 
{
    bool found = false;
    for(const auto& client : clients) 
    {
        if(client.second.name == recipientName) 
        {
            found = true;
            string formattedMessage = "[PM from " + clients[senderSocket].name + "]: " + message;
            send(client.first, (char*)&COLORS[13], sizeof(int), 0);
            send(client.first, formattedMessage.c_str(), formattedMessage.length(), 0);
            
            string confirmation = "[PM to " + recipientName + "]: " + message;
            send(senderSocket, (char*)&COLORS[13], sizeof(int), 0);
            send(senderSocket, confirmation.c_str(), confirmation.length(), 0);
            break;
        }
    }
    
    if(!found) 
    {
        string errorMsg = "User '" + recipientName + "' not found.";
        send(senderSocket, (char*)&COLORS[12], sizeof(int), 0);
        send(senderSocket, errorMsg.c_str(), errorMsg.length(), 0);
    }
}

void broadcastMessage(const string& senderName, const string& message, SOCKET senderSocket, int color) 
{
    string formattedMessage = senderName + ": " + message;
    for(SOCKET client : activeClients) 
    {
        if(client != senderSocket) 
        {
            send(client, (char*)&color, sizeof(int), 0);
            send(client, formattedMessage.c_str(), formattedMessage.length(), 0);
        }
    }
}

void ProcessNewMessage(SOCKET nClientSocket) 
{
    char buff[256 + 1] = {0,};
    int nRet = recv(nClientSocket, buff, 256, 0);

    if(nRet < 0) 
    {
        cout << "\nError occurred, closing connection for client: " << nClientSocket << endl;
        
        auto it = std::find(activeClients.begin(), activeClients.end(), nClientSocket);
        if(it != activeClients.end()) 
        {
            activeClients.erase(it);
        }
        
        string disconnectMsg = clients[nClientSocket].name + " has left the chat.";
        for(SOCKET client : activeClients) 
        {
            if(clients.find(client) != clients.end() && !clients[client].name.empty()) 
            {
                send(client, (char*)&COLORS[14], sizeof(int), 0);
                send(client, disconnectMsg.c_str(), disconnectMsg.length(), 0);
            }
        }
        
        clients.erase(nClientSocket);
        closesocket(nClientSocket);
    }
    else 
    {
        string message(buff);
        if(clients[nClientSocket].name.empty()) 
        {
            clients[nClientSocket].name = message;
            clients[nClientSocket].color = COLORS[currentColorIndex];
            currentColorIndex = (currentColorIndex + 1) % (sizeof(COLORS) / sizeof(COLORS[0]));
            
            const char* welcome = "Welcome to Theta Chat! You can start chatting now.\n";
            send(nClientSocket, (char*)&COLORS[10], sizeof(int), 0);
            send(nClientSocket, welcome, strlen(welcome), 0);

            string welcomeMsg = message + " has joined the chat!\n";
            for(SOCKET client : activeClients) 
            {
                if(clients.find(client) != clients.end() && !clients[client].name.empty()) 
                {
                    send(client, (char*)&COLORS[14], sizeof(int), 0);
                    send(client, welcomeMsg.c_str(), welcomeMsg.length(), 0);
                }
            }
        }
        else 
        {
            if(message.substr(0, 4) == "/msg") 
            {
                size_t spacePos = message.find(' ', 4);
                if(spacePos != string::npos) 
                {
                    string recipient = message.substr(4, spacePos - 4);
                    while(recipient[0] == ' ') recipient = recipient.substr(1);
                    string privateMessage = message.substr(spacePos + 1);
                    while(privateMessage[0] == ' ') privateMessage = privateMessage.substr(1);
                    sendPrivateMessage(nClientSocket, recipient, privateMessage);
                }
                else 
                {
                    string errorMsg = "Invalid format. Use: /msg username message";
                    send(nClientSocket, (char*)&COLORS[12], sizeof(int), 0);
                    send(nClientSocket, errorMsg.c_str(), errorMsg.length(), 0);
                }
            }
            else if(message == "/list") 
            {
                string memberList = "\n<--- Online Members --->\n";
                for(const auto& client : clients) 
                {
                    if(!client.second.name.empty()) 
                    {
                        memberList += client.second.name + "\n";
                    }
                }
                memberList += "<------------------->\n";
                send(nClientSocket, (char*)&COLORS[11], sizeof(int), 0);
                send(nClientSocket, memberList.c_str(), memberList.length(), 0);
            } 
            else if(message == "/help") 
            {
                string featuresList = "<--- Available Commands --->\n";
                featuresList += "/list - Show all online members\n";
                featuresList += "/clear - Clear your chat screen\n";
                featuresList += "/time - Show current server time\n";
                featuresList += "/count - Show total number of online users\n";
                featuresList += "/help - Show this help message\n";
                featuresList += "/msg <Username> - Send private message\n";
                send(nClientSocket, (char*)&COLORS[10], sizeof(int), 0);
                send(nClientSocket, featuresList.c_str(), featuresList.length(), 0);
            } 
            else if(message == "/clear") 
            {
                string clearMsg = "Chat screen cleared.\n";
                send(nClientSocket, (char*)&COLORS[10], sizeof(int), 0);
                send(nClientSocket, clearMsg.c_str(), clearMsg.length(), 0);
            } 
            else if(message == "/time") 
            {
                time_t now = time(0);
                char* dt = ctime(&now);
                string timeMsg = "\nCurrent server time: " + string(dt);
                send(nClientSocket, (char*)&COLORS[13], sizeof(int), 0);
                send(nClientSocket, timeMsg.c_str(), timeMsg.length(), 0);
            } 
            else if(message == "/count") 
            {
                int count = 0;
                for(const auto& client : clients) 
                {
                    if(!client.second.name.empty()) count++;
                }
                string countMsg = "\nTotal online users: " + to_string(count) + "\n";
                send(nClientSocket, (char*)&COLORS[12], sizeof(int), 0);
                send(nClientSocket, countMsg.c_str(), countMsg.length(), 0);
            } 
            else 
            {
                string formattedMessage = clients[nClientSocket].name + ": " + message;
                for(SOCKET client : activeClients) 
                {
                    if(client != nClientSocket && clients.find(client) != clients.end() && !clients[client].name.empty()) 
                    {
                        send(client, (char*)&clients[nClientSocket].color, sizeof(int), 0);
                        send(client, formattedMessage.c_str(), formattedMessage.length(), 0);
                    }
                }
            }
        }
    }
}

void ProcessTheNewRequest() 
{
    if(FD_ISSET(nSocket, &fr)) 
    {
        struct sockaddr_in clientAddr;
        int nLen = sizeof(clientAddr);
        SOCKET nClientSocket = accept(nSocket, (struct sockaddr *)&clientAddr, &nLen);

        if(nClientSocket != INVALID_SOCKET) 
        {
            activeClients.push_back(nClientSocket);
            const char* prompt = "Please enter your name: ";
            send(nClientSocket, (char*)&COLORS[11], sizeof(int), 0);
            send(nClientSocket, prompt, strlen(prompt), 0);

            if(nClientSocket > nMaxFd) 
            {
                nMaxFd = nClientSocket;
            }
        } 
        else 
        {
            cout << "\nError accepting connection\n";
        }
    } 
    else 
    {
        for(SOCKET clientSocket : activeClients) 
        {
            if(FD_ISSET(clientSocket, &fr)) 
            {
                ProcessNewMessage(clientSocket);
            }
        }
    }
}

int main() 
{
    WSADATA wsaData;
    int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(nRet != 0) 
    {
        cout << "WSAStartup failed\n";
        return 1;
    }

    nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(nSocket == INVALID_SOCKET) 
    {
        cout << "\nSocket could not be opened\n";
        WSACleanup();
        return 1;
    } 
    else 
    {
        cout << "\nSocket opened successfully\n";
    }

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = INADDR_ANY;
    memset(&(srv.sin_zero), 0, 8);

    int optVal = 1;
    nRet = setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, sizeof(optVal));

    if(nRet == 0) 
    {
        cout << "\nSetsockopt call successful\n";
    } 
    else 
    {
        cout << "\nFailed\n";
        closesocket(nSocket);
        WSACleanup();
        return 1;
    }

    nRet = bind(nSocket, (sockaddr*)&srv, sizeof(sockaddr));

    if(nRet == SOCKET_ERROR) 
    {
        cout << "\nFailed to bind to the local port\n";
        closesocket(nSocket);
        WSACleanup();
        return 1;
    } 
    else 
    {
        cout << "\nSuccessfully bound to local port\n";
    }

    nRet = listen(nSocket, 5);
    if(nRet == SOCKET_ERROR) 
    {
        cout << "\nFailed to listen to the local port\n";
        closesocket(nSocket);
        WSACleanup();
        return 1;
    } 
    else 
    {
        cout << "\nStarted listening to the local port\n";
    }

    nMaxFd = nSocket;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while(true) 
    {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);

        FD_SET(nSocket, &fr);
        FD_SET(nSocket, &fe);

        for(SOCKET clientSocket : activeClients) 
        {
            FD_SET(clientSocket, &fr);
            FD_SET(clientSocket, &fe);
        }

        nRet = select(nMaxFd + 1, &fr, &fw, &fe, &tv);
        if(nRet > 0) 
        {
            ProcessTheNewRequest();
        } 
        else if(nRet == 0) 
        {
        } 
        else 
        {
            cout << "Nothing in PORT: " << PORT << endl;
        }
    }

    closesocket(nSocket);
    WSACleanup();
    return 0;
}
