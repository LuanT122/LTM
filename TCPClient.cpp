

#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>


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
    } while (byteReceived == MAX);
    return totalReceived;
}

int main()
{
    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);
    SOCKET c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    saddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    int result = connect(c, (sockaddr*)&saddr, sizeof(saddr));
    if (result != SOCKET_ERROR) {
        char data[1000];
        printf("Nhap vao du lieu gui den server: ");
        fflush(stdin);
        fgets(data, 1000, stdin);
        SendData(c, data, strlen(data));
        char* buffer = NULL;
        ReceiveData(c, &buffer);
        printf("Du lieu tra ve tu server: %s\n", buffer);
        free(buffer);
        buffer = NULL;
        closesocket(c);
    }
    WSACleanup();
}

