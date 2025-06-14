void ProcessNewMessage(SOCKET nClientSocket) {
    char buff[256 + 1] = {0,};
    int nRet = recv(nClientSocket, buff, 256, 0);
    if (nRet < 0) {
        cout << "\nError occurred, closing connection for client: " << nClientSocket << endl;
        closesocket(nClientSocket);
        exit(EXIT_FAILURE);
    } else {
        // Get color first
        int color;
        recv(nClientSocket, (char*)&color, sizeof(int), 0);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        cout << buff << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

int main() {
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
        cout << "WSAStartup failed" << endl;
        return EXIT_FAILURE;
    }

    SOCKET nClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nClientSocket < 0) {
        cout << "Socket creation failed" << endl;
        return EXIT_FAILURE;
    }

    struct sockaddr_in srv;
    srv.sin_family = AF_INET;
    srv.sin_port = htons(9001);
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");

    int nRet = connect(nClientSocket, (struct sockaddr*)&srv, sizeof(srv));
    if (nRet < 0) {
        cout << "Connection failed" << endl;
        return EXIT_FAILURE;
    }

    cout << "Connected to server!" << endl;
    cout << "Enter your name: ";
    string name;
    getline(cin, name);
    send(nClientSocket, name.c_str(), name.length(), 0);

    // Display help message
    cout << "\n<--- Theta Chat Commands --->" << endl;
    cout << "/members or /list - Show all online members" << endl;
    cout << "/clear - Clear your chat screen" << endl;
    cout << "/time - Show current server time" << endl;
    cout << "/count - Show total number of online users" << endl;
    cout << "/help or /features - Show all available commands" << endl;
    cout << "<------------------------>\n" << endl;

    thread t1(ProcessNewMessage, nClientSocket);
    t1.detach();

    while (true) {
        string message;
        getline(cin, message);
        send(nClientSocket, message.c_str(), message.length(), 0);
    }

    closesocket(nClientSocket);
    WSACleanup();
    return 0;
} 