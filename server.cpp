#include <stdio.h>
#include <WinSock2.h>
CRITICAL_SECTION cs;

DWORD WINAPI ClientThread(LPVOID param) {
    SOCKET c = (SOCKET)param;
    char user[1024];
    char password[1024];
    memset(user, 0, sizeof(user));
    memset(password, 0, sizeof(password));
    char buffer[1024];

    char* welcome = (char*)"Enter user: ";
    send(c, welcome, strlen(welcome), 0);
    memset(buffer, 0, sizeof(buffer));
    recv(c, buffer, sizeof(buffer), 0);
    while (buffer[strlen(buffer) - 1] == '\r' ||
        buffer[strlen(buffer) - 1] == '\n')
    {
        buffer[strlen(buffer) - 1] = 0;
    }
    sscanf(buffer, "%s", user);

    welcome = (char*)"Enter password: ";
    send(c, welcome, strlen(welcome), 0);
    memset(buffer, 0, sizeof(buffer));
    recv(c, buffer, sizeof(buffer), 0);
    while (buffer[strlen(buffer) - 1] == '\r' ||
        buffer[strlen(buffer) - 1] == '\n')
    {
        buffer[strlen(buffer) - 1] = 0;
    }
    sscanf(buffer, "%s", password);
    
    FILE* f = fopen("C:\\server.txt", "rt");
    int matched = 0;
    while(!feof(f)) 
        {
        char line[1024], _u[1024], _p[1024];
        memset(line, 0, sizeof(line));
        fgets(line, sizeof(line), f);
        while (line[strlen(line) - 1] == '\r' ||
            line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = 0;
        }
        memset(_u, 0, sizeof(_u));
        sscanf(line, "%s", _u);

        memset(line, 0, sizeof(line));
        fgets(line, sizeof(line), f);
        while (line[strlen(line) - 1] == '\r' ||
            line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = 0;
        }
        memset(_p, 0, sizeof(_p));
        sscanf(line, "%s", _p);

        if(strcmp(user, _u) == 0 && 
            strcmp(password, _p) == 0)
            {
                matched = 1;
                break;
            } 
        } 
    fclose(f);

    if (matched == 0)
    {
        char* failed = (char*)"Failed to login!...\n";
        send(c, failed, strlen(failed), 0);
        closesocket(c);
    }
    else
    {   
        char* succeeded = (char*)"Please send command to execute: ";
        send(c, succeeded, strlen(succeeded), 0);
        memset(buffer, 0, sizeof(buffer));
        recv(c, buffer, sizeof(buffer), 0);
        while (buffer[strlen(buffer) - 1] == '\r' ||
            buffer[strlen(buffer) - 1] == '\n')
        {
            buffer[strlen(buffer) - 1] = 0;
        }
        sprintf(buffer + strlen(buffer), "%s", "> C:\\out.txt");
        LeaveCriticalSection(&cs);
        // Lay du lieu gui ve
        EnterCriticalSection(&cs);
        system(buffer);
        FILE* f = fopen("C:\\out.txt", "rb");
        fseek(f, 0, SEEK_END);  // Dua con tro ve cuoi file
        int flen = ftell(f);    // Lay vi tri con tro
        fseek(f, 0, SEEK_SET);  // Dua con tro ve lai dau file
        char* fdata = (char*)calloc(flen, 1);
        fread(fdata, 1, flen, f);
        fclose(f);
        LeaveCriticalSection(&cs);
        send(c, fdata, flen, 0);
        free(fdata);
        closesocket(c);
    }
    return 0;
}

int main() 
{   
   
    InitializeCriticalSection(&cs);
    
    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);
    SOCKADDR_IN saddr;
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8000);
    saddr.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(s, (sockaddr*)&saddr, sizeof(saddr));
    listen(s, 10);

        while (0 == 0) {
            SOCKADDR_IN caddr;
            int clen = sizeof(caddr);
            SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
            CreateThread(NULL, 0, ClientThread, (LPVOID)c, 0, NULL);
        }
    DeleteCriticalSection(&cs);
    
    return 0;
}

