#include <stdio.h>
#include <WinSock2.h>

int SendData(SOCKET c, char* data, int len) {
    int byteSent = 0;
    while (byteSent < len) {
        byteSent += send(c, data + byteSent, len - byteSent, 0);
    }
    return byteSent;
}

int ReceiveData(SOCKET c, char** buffer) {
    const int MAX = 1024;
    int totalReceived = 0;
    char tmp[MAX];
    int byteReceived = 0;
    do {
        byteReceived = recv(c, tmp, MAX, 0);
            if (byteReceived > 0) {
                *buffer = (char*)realloc(*buffer, totalReceived + byteReceived + 1);
                memset(*buffer + totalReceived, 0, byteReceived + 1);
                memcpy(*buffer + totalReceived, tmp, byteReceived);
                totalReceived += byteReceived;
            }
    } while(byteReceived == MAX);
    return totalReceived;
}

int main()
{
    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (s == INVALID_SOCKET) {
        printf("khong tao duoc socket, ma loi: %d\n", WSAGetLastError());
        return 0;
    }
    else {
        SOCKADDR_IN saddr;
        saddr.sin_family = AF_INET;
        saddr.sin_port = 80;
        saddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        bind(s, (sockaddr*)&saddr, sizeof(saddr));
        listen(s, 10);

        SOCKADDR_IN caddr;
        int clen = sizeof(caddr);
        SOCKET c = accept(s, (sockaddr*)&caddr, &clen);

        int errorCode = WSAGetLastError();
        char* hello = (char*)"Hello Programming\n";
        //int byteSent = send(c, hello, strlen(hello), 0);
        //printf("So byte da gui: %d\n", byteSent);

        int byteSent = SendData(c, hello, strlen(hello));
        printf("So byte da gui: %d\n", byteSent);
        //char data[1024]; 
        //memset(data, 0, sizeof(data));
        char* data = NULL;
        //int byteReceived = recv(c, data, 1024, 0)
        //printf("So byte da nhan: %d\n", byteReceived);;
        int byteReceived = ReceiveData(c, &data);
        printf("So byte nhan duoc: %d\n", byteReceived);
        printf("Du lieu nhan duoc: %s\n", data);

        free(data);
        data = NULL;
        closesocket(c);
        closesocket(s);
        WSACleanup();
    }
}

