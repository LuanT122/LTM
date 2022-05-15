#include <iostream>
#include<WinSock2.h>
#include <iterator>
#include<map>
using namespace std;
#define MAX_CLIENT 1024

SOCKET clients[MAX_CLIENT];
int mark = 0;


DWORD WINAPI ClientThread(LPVOID param);
char ClientId[MAX_CLIENT][1024];
int main()
{
    
    int count = 0; //so luong ket noi
    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);


    SOCKADDR_IN saddr;
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    saddr.sin_addr.S_un.S_addr = INADDR_ANY;
    saddr.sin_port = htons(8000);
    saddr.sin_family = AF_INET;
    bind(s, (sockaddr*)&saddr, sizeof(saddr));
    listen(s, 10);
    fd_set fdread;
    while (1) {
        FD_ZERO(&fdread); // xoa het cac socket di
        //Bo socket le tan vao tap tham do
        FD_SET(s, &fdread);
        /// <summary>
        ///Tao tap set tham do chua cac socket client 
        /// </summary>
        /// <returns></returns>
        for (int i = 0; i < count; i++)
        {
            FD_SET(clients[i], &fdread);
        }

        select(0, &fdread, NULL, NULL, NULL);

        //Xac dinh co ket noi den socket le tan (socket s) va tien hanh them vao tap socket client
        if (FD_ISSET(s, &fdread)) {
            SOCKADDR_IN caddr;
            int clen = sizeof(caddr);
            SOCKET tmp = accept(s, (sockaddr*)&caddr, &clen);

            //=========================
            //Chuc nang dang nhap de xac nhan client ket noi
            //===========================
            SOCKET c = (SOCKET)tmp; // do tham chieu den socket c nen can ep kieu

            //Gui chuoi ky tu chao mung huong dan client nhap du lieu
            char* welcome = (char*)"Gui client_id: theo dinh dang \n";
            send(c, welcome, strlen(welcome), 0);
            //================
        //Dang nhap
        //=====================
            while (mark == 0) {
                //Tao ra chuoi ky tu buffer dung de nhan thong tin dang nhap tu phia client gui len server
                char buffer[1024];
                
                //Nhan xau ky tu chua clients_id: tu phia client gui len
                recv(c, buffer, sizeof(buffer), 0);
                //xu ly khoang trang (TRIM)
                while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n') {
                    buffer[strlen(buffer) - 1] = 0;
                }
               
                char command[1024];
                char clientId[1024];
                memset(command, 0, sizeof(command));
                memset(command, 0, sizeof(clientId));
                sscanf(buffer, "%s%s", command, clientId);
                if (strcmp("client_id:", command) == 0) {
                    mark = 1;
                    char* succeded = (char*)"login thanh cong \n";
                    send(c, succeded, strlen(succeded), 0);
                    
                    memset(ClientId[count], 0, sizeof(ClientId[count]));
                    strcat(ClientId[count], clientId);
                   
                    clients[count] = c;
                    count += 1;
                }
                else {
                    //Tao va gui chuoi thong bao ket qua dang nhap thanh cong ve cho client
                    mark = 0;
                    char* failed = (char*)"login that bai \n Cu phap client_id: \n";
                    send(c, failed, strlen(failed), 0);
                   
                    //Khong dong socket khi gui sai cu phap
                    
                }
            }
            
        }
        
                //duyet lang nghe tat ca cac client
        char idData[1024];
        for (int i = 0; i < count; i++) {
            //Neu client[i] muon gui tin thi nhan tin nhan vao chuoi buffer
            if (FD_ISSET(clients[i], &fdread)) {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                memset(idData, 0, sizeof(idData));
                strcat(idData,ClientId[i]);
                recv(clients[i], buffer, sizeof(buffer), 0);

                if (strcmp(buffer, "quit\n") == 0) {
                    closesocket(clients[i]);
                    for (int k = i; i < count-1; i++) {
                        clients[i] = clients[i + 1];
                    }
                    count -= 1;
                }
                strcat(idData, ": ");
                strcat(idData, buffer);
                for (int j = 0; j < count; j++) {
                    //Sau khi nhan tin nhan thi duyet toan bo mang client, neu khong phai client gui tin thi tien hanh gui thong diep ma server nhan duoc
                    if (j != i) {
                        
                        send(clients[j], idData, strlen(idData), 0);
                    }
                }
            }
        }
    }
}

