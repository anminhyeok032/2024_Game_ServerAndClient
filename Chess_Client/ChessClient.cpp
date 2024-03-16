// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "Common.h"
#include "framework.h"
#include "WindowsProject1.h"
#include <atlImage.h>


#define MAX_LOADSTRING 100
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

SOCKET sock;
SOCKET server_s;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    //========================================================
    // 연결할 네트워크 주소 입력
    char SERVER_ADDR[BUFSIZE];
    std::cout << "Enter IP Address : ";
    std::cin.getline(SERVER_ADDR, BUFSIZE);

    // 네트워크 초기화
    int res;
    std::wcout.imbue(std::locale("korean"));

    WSADATA WSAData;
    res = WSAStartup(MAKEWORD(2, 2), &WSAData);
    if (0 != res)
    {
        print_error("WSAStartup", WSAGetLastError());
    }

    server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
    SOCKADDR_IN server_a;
    server_a.sin_family = AF_INET;
    server_a.sin_port = htons(SERVERPORT);
    inet_pton(AF_INET, SERVER_ADDR, &server_a.sin_addr);

    res = connect(server_s, reinterpret_cast<sockaddr*> (&server_a), sizeof(server_a));
    if (SOCKET_ERROR == res)
    {
        print_error("Connect", WSAGetLastError());
        closesocket(server_s);
        WSACleanup();
        return 1;
    }
    //========================================================

    MSG msg;

    // 기본 메시지 루프입니다:
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
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, L"2019314019 HW01", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
       CW_USEDEFAULT, 0, 800, 800, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static CImage img, imgSprite;
    
    static Coordinate coord{0, 0 };
    static RECT rect;

    // 키입력 버퍼
    CHAR keyInput = 0;

    switch (message)
    {
    case WM_CREATE:
	    {
			img.Load(TEXT("Image\\chess_board.bmp"));
            imgSprite.Load(TEXT("Image\\chess_player.png"));
	    }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            int w = img.GetWidth();
            int h = img.GetHeight();

	        GetClientRect(hWnd, &rect);

	        HDC hdc = GetDC(hWnd); // 그림 그릴 윈도우의 DC 가져오기
	        HDC hMemdc = CreateCompatibleDC(hdc); // 화면dc(그릴 윈도우) 기반 메모리 DC생성 
	        HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom); // 비트맵 생성
	        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemdc, hBitmap); // 비트맵 선택

            //FillRect(hMemdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
            img.Draw(hMemdc, 0, 0, rect.right, rect.bottom, 0, 0, w, h); //--- 메모리 DC에 배경 그리기
            imgSprite.Draw(hMemdc, coord.x, coord.y, 100, 100, 0, 0, imgSprite.GetWidth(), imgSprite.GetHeight());
    		BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemdc, 0, 0, SRCCOPY); // 고속복사 hMemdc->hdc

	        SelectObject(hMemdc, hOldBitmap); // 기존 비트맵 선택
            DeleteDC(hMemdc); // 메모리 dc 해제
            DeleteDC(hdc); // 화면 dc 해제
        }
        break;

    case WM_KEYDOWN:
        {

            CHAR keyInput = static_cast<CHAR>(wParam);
            WSABUF wsabuf[1];
            wsabuf[0].buf = reinterpret_cast<CHAR*>(&keyInput);
            wsabuf[0].len = sizeof(CHAR);

            DWORD sent_size;

            int ret = WSASend(server_s, wsabuf, 1, &sent_size, 0, nullptr, nullptr);
            if (ret == SOCKET_ERROR)
            {
                print_error("WSASend", WSAGetLastError());
            }


            char recvBuffer[BUFSIZE];
            wsabuf[0].buf = recvBuffer;
            wsabuf[0].len = BUFSIZE;
            DWORD bytesReceived;
            DWORD recv_flag = 0;

            ret = WSARecv(server_s, wsabuf, 1, &bytesReceived, &recv_flag, nullptr, nullptr);
            if (ret == SOCKET_ERROR)
            {
                print_error("WSARecv", WSAGetLastError());
            }

            memcpy(&coord, recvBuffer, sizeof(Coordinate));
            printf("Received Coordinates from server: x = %d, y = %d\n", coord.x, coord.y);
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

// 정보 대화 상자의 메시지 처리기입니다.
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
