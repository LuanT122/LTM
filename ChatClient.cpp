// ChatClient.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "ChatClient.h"
#include "winsock2.h"
#include <stdio.h>
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

#define WM_SOCKET WM_USER + 1
#define MAX_LOADSTRING 100


// Global Variables:
HINSTANCE hInst, hInst2;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
SOCKET clients[64];
int numClients = 1;
HWND hWnd1;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
ATOM                MyRegisterClass2(HINSTANCE hInstance);
BOOL                InitInstance2(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc2(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
SOCKET listener;
SOCKET Client;
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Tao socket cho ket noi
    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Tao dia chi server
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);
    // Gan socket voi cau truc dia chi
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    // Chuyen socket sang trang thai cho ket noi
    listen(listener, 5);


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHATCLIENT, szWindowClass, MAX_LOADSTRING);
    
  
    
    MyRegisterClass2(hInstance);

    // Perform application initialization:
    if (!InitInstance2 (hInstance, nCmdShow))
    {
        return FALSE;
    }
    
    /*  MyRegisterClass(hInstance);*/
  /* if (!InitInstance(hInstance, SW_MINIMIZE))
    {
        return FALSE;
    }*/
    

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATCLIENT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHATCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM MyRegisterClass2(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex2;

    wcex2.cbSize = sizeof(WNDCLASSEX);

    wcex2.style = CS_HREDRAW | CS_VREDRAW;
    wcex2.lpfnWndProc = WndProc2;
    wcex2.cbClsExtra = 0;
    wcex2.cbWndExtra = 0;
    wcex2.hInstance = hInstance;
    wcex2.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATCLIENT));
    wcex2.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex2.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex2.lpszMenuName = MAKEINTRESOURCEW(IDC_CHATCLIENT);
    wcex2.lpszClassName = szWindowClass;
    wcex2.hIconSm = LoadIcon(wcex2.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    
    return RegisterClassExW(&wcex2);
}
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
        
    hInst = hInstance; // Store instance handle in our global variable

    hWnd1 = CreateWindowW(szWindowClass, L"Chat", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd1)
    {
        return FALSE;
    }
    WSAAsyncSelect(listener, hWnd1, WM_SOCKET, FD_ACCEPT);


    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
        10, 10, 400, 350, hWnd1, (HMENU)IDC_LIST_MESSAGE, hInst, NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
        420, 10, 150, 350, hWnd1, (HMENU)IDC_LIST_CLIENT, hInst, NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        10, 360, 400, 40, hWnd1, (HMENU)IDC_EDIT_MESSAGE, hInst, NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("SEND"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
        420, 360, 150, 40, hWnd1, (HMENU)IDC_BUTTON_SEND, hInst, NULL);



    


    ShowWindow(hWnd1, nCmdShow);
    UpdateWindow(hWnd1);
    SendDlgItemMessageA(hWnd1, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)"ALL");
    return TRUE;
}

BOOL InitInstance2(HINSTANCE hInstance, int nCmdShow)
{
   hInst2 = hInstance; // Store instance handle in our global variable

   HWND hWnd2 = CreateWindowW(szWindowClass, L"Log in", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   WSAAsyncSelect(listener, hWnd2, WM_SOCKET, FD_ACCEPT);
   
   if (!hWnd2)
   {
      return FALSE;
   }

   ShowWindow(hWnd2, nCmdShow);
   UpdateWindow(hWnd2);

   
   
    CreateWindowEx(WS_EX_TRANSPARENT, TEXT("STATIC"), TEXT("Enter user id to log in:"), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
       10, 125, 400, 20, hWnd2, (HMENU)IDC_STATIC, GetModuleHandle(NULL), NULL);
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
       10, 150, 400, 40, hWnd2, (HMENU)IDC_EDIT_ID, GetModuleHandle(NULL), NULL);
     
    CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Login"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
       125, 200, 150, 40, hWnd2, (HMENU)IDC_BUTTON_LOGIN, GetModuleHandle(NULL), NULL);
    
   return TRUE;

}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SOCKET:
    {
        if (WSAGETSELECTERROR(lParam))
        {
            closesocket(wParam);
        }
        else
        {
            if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT)
            {
                SOCKET client = accept(wParam, NULL, NULL);
                WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
                clients[numClients] = client;
                numClients++;
                char cid[32];
                itoa(client, cid, 10);
                SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)cid);
            }
            else if (WSAGETSELECTEVENT(lParam) == FD_CONNECT)
            {
                SOCKET client = socket(wParam, NULL, NULL);
                WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);

                while (1)
                {
                    char buf[256];
                    int ret = recv(client, buf, sizeof(buf), 0);
                    if (ret <= 0)
                    {
                        if (ret == -1)
                            ret = WSAGetLastError();
                        break;
                    }
                    if (ret < sizeof(buf))
                        buf[ret] = 0;
                    printf("%s", buf);


                    char cid[32];
                    itoa(client, cid, 10);
                    SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)cid);

                }
            }
            
            else if (WSAGETSELECTEVENT(lParam) == FD_READ)
            {
                char buf[256];
                int ret = recv(wParam, buf, sizeof(buf), 0);
                buf[ret] = 0;
                SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, LB_ADDSTRING, 0, (LPARAM)buf);
            }
            else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
            {
                closesocket(wParam);
            }
        }
        }
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDC_BUTTON_SEND:
        {
            char buf[256];
            GetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, buf, sizeof(buf));
            int i = SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_GETCURSEL, 0, 0);
            if (i == 0) 
            {
                for (int i = 1;i < numClients; i++) 
                {
                    send(clients[i], buf, strlen(buf), 0);
                }
            }else
            send(clients[i], buf, strlen(buf), 0);
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}



LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


    switch (message)
    {
       

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDC_BUTTON_LOGIN:
              
        {   
        
           Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            MyRegisterClass(hInst);
            
            if (!InitInstance(hInst, SW_SHOW))
            {
                return FALSE;
            }
            SOCKADDR_IN addr;
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            addr.sin_port = htons(8000);
            

            int ret = connect(Client, (SOCKADDR*)&addr, sizeof(addr));

            if (ret == SOCKET_ERROR)
            {
                ret = WSAGetLastError();
                printf("Ket noi khong thanh cong - %d\n", ret);
                return 1;
                break;
            }
            
            const char* msg = "Hello server!!!\n";
            send(Client, msg, strlen(msg), 0);
            
            WSAAsyncSelect(Client, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
                  

            break;

        }
        default:
            return DefWindowProc(hWnd1, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
