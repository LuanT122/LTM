#include <winsock2.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)
//tao danh sach
struct node {
    const char* id;
    SOCKET socket;
    node* next;
    node* pre;
};
struct douList {
    node* head;
    node* tail;
};
douList* createList(const char* s,SOCKET socket) {
    douList* l = new douList;
    l->head = new node;
    l->head->id = s;
    l->head->socket = socket;
    l->head->pre = NULL;
    l->head->next = NULL;
    l->tail = l->head;
    return l;
}
douList* addTail(douList* l, const char* s,SOCKET socket) {
    node* temp = new node;
    temp->id = s;
    temp->socket = socket;
    temp->next = NULL;
    temp->pre = l->tail;
    l->tail->next = temp;
    l->tail = temp;
    return l;
}

douList* deleteAt(douList* l, string id) {
    node* p = l->head;
    while(p != NULL) {
        if (p->id == id) {
            node* temp = p->next;
            node* p2 = p->pre;
            p2->next = temp;
            temp->pre = p2;
            delete(p);
            return l;

        }
        p = p->next;
    }
    
}

node* search(douList* l, string id) {
    node* p = l->head;
    while (p != NULL) {
        if (p->id == id) {
            return p;
        }
        p = p->next;
        
    }
}




void RemoveClient(SOCKET* clients, int* numClients, int i)
{
// Xoa client khoi mang
    if (i < *numClients - 1)
        clients[i] = clients[*numClients - 1];
    *numClients--;
}


int main()
{
    // Khoi tao thu vien
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    // Tao socket
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Khai bao dia chi server
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);
    fd_set fdread;
    int ret;
    SOCKET clients[64];
    int numClients = 0;
    SOCKET connected[64];
    int numConnected = 0;
    SOCKET ad{};
    //tao danh sach
    douList* LIST =createList("ad",ad);

    

    
   
    char buf[256];
    char cmd[32], id[32], tmp[32];
    int x = 0;
    fstream f;
    f.open("D:\\List\\File.txt");

    string s;
    while (1)
    {

        FD_ZERO(&fdread);
        FD_SET(listener, &fdread);

        

        for (int i = 0; i < numClients; i++)
            FD_SET(clients[i], &fdread);
        ret = select(0, &fdread, 0, 0, 0);
        if (ret > 0)
        {
            // Kiem tra su kien ket noi moi
            if (FD_ISSET(listener, &fdread))
            {
                SOCKET client = accept(listener, NULL, NULL);
                printf("Ket noi moi: %d\n", client);
                const char* msg = "Hello client\n";
                send(client, msg, strlen(msg), 0);
                clients[numClients] = client;
                numClients++;
            }
            for (int i = 0; i < numClients; i++)
            {
                if (FD_ISSET(clients[i], &fdread))
                {
                    ret = recv(clients[i], buf, sizeof(buf), 0);
                    if (ret <= 0)
                    {
                        // Xoa client ra khoi mang clients
                        RemoveClient(clients, &numClients, i);
                        i--;
                        continue;
                    }
                    buf[ret] = 0;
                    printf("Du lieu nhan duoc tu %d: %s\n", clients[i], buf);
                    // Kiem tra trang thai dang nhap
                    SOCKET client = clients[i];
                    int j = 0;
                    for (; j < numConnected; j++)
                        if (connected[j] == client) break;
                    if (j == numConnected)
                    {
                        // Chua dang nhap
                        ret = sscanf(buf, "%s %s %s", cmd, id, tmp);

                        if (ret != 2)
                        {
                            const char* msg = "Sai cu phap. Hay nhap lai.\n";
                            send(client, msg, strlen(msg), 0);
                        }
                        else
                        {
                            if (strcmp(cmd, "client_id:") != 0)
                            {
                                const char* msg = "Sai cu phap. Hay nhap lai.\n";
                                send(client, msg, strlen(msg), 0);
                            }
                            else
                            {
                                
                                while (!f.eof()) {  
                                    
                                    getline(f, s);
                                    if (s != id)
                                    {
                                        const char* msg = "....\n";
                                        //send(client, msg, strlen(msg), 0);
                                    }
                                    else {
                                        const char* msg = "Dang nhap thanh cong. Nhap tin nhan de chuyen tiep.\n";
                                        send(client, msg, strlen(msg), 0);
                                        // Them vao mang
                                        connected[numConnected] = client;
                                        numConnected++;

                                        const char* msg1 = "Dang nhap\n";
                                        char sbuf[256];
                                        sprintf(sbuf, "%s: %s", id, msg1);

                                        for (int j = 0; j < numConnected; j++)
                                            send(connected[j], sbuf, strlen(sbuf), 0);

                                        addTail(LIST, id, client);
                                        x =  1;

                                        f.close();
                                        f.open("D:\\List\\File.txt");

                                        break;
                                    }
                                }
                                if (x==0) {
                                    const char* msg = "Tk k ton tai.\n";
                                    send(client, msg, strlen(msg), 0);
                                    
                                    f.close();
                                     f.open("D:\\List\\File.txt");
                                }
                                

                                
                            }
                        }
                    }
                    else
                    {
                        // Da dang nhap
                        char sbuf[256];
                        sprintf(sbuf, "%s: %s", id, buf,tmp);
                        sscanf(sbuf, "%s %s", id, buf,tmp);

                        

                        if (strcmp(buf, "DISCONNECT")==0) {
                            closesocket(client);
                            for (int j = 0; j < numConnected; j++)
                                send(connected[j], sbuf, strlen(sbuf), 0);
                        }
                        else if (strcmp(buf, "ALL") == 0) {
                            for (int j = 0; j < numConnected; j++)
                                if (client != connected[j])
                                {
                                    send(connected[j], sbuf, strlen(sbuf), 0);
                                }
                        }

                        else if (strcmp(buf, "SEND_TO")==0) {
                            //gui cho client co ID xac dinh

                            const char* msg = "Gui den ID:\n";

                            send(client, msg, strlen(msg), 0);

                            ret = recv(clients[i], buf, sizeof(buf), 0);
                            if (search(LIST, buf)) {
                                ret = recv(clients[i], buf, sizeof(buf), 0);
                                send(search(LIST, buf)->socket, buf, strlen(buf), 0);
                                
                            }
                            else {
                                const char* msg = "TK k ton tai\n";

                                send(client, msg, strlen(msg), 0);

                            }
                                
                                

                        }

                        else if (strcmp(buf, "LIST") == 0) {
                            node* p = LIST->head;
                            while (p != NULL) {
                                send(client,p->id, strlen(p->id), 0);
                                send(client, "-", 1, 0);

                                p = p->next;
                            }

                            
                        }
                        else {
                            const char* msg = "Sai cu phap\n";
                            send(client, msg, strlen(msg), 0);
                        }
                        
                    }
                }
            }
        }
    }
}
