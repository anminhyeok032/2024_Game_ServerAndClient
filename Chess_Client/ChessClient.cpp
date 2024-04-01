// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "Common.h"
#include "framework.h"
#include "WindowsProject1.h"
#include "ChessPlayer.h"


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

void CALLBACK recv_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK send_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

SOCKET server_s;
SOCKADDR_IN server_a;
std::unordered_map<int, ChessPlayer> g_players;
bool bCheckConnect = false;
//char buf[BUFSIZE];

WSAOVERLAPPED wsaover;
WSABUF wsabuf;
ChessPlayer player;
char recvBuffer[sizeof(int)];



void print_error(const char* msg, int err_no)
{
    WCHAR* msg_buf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err_no,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
    std::cout << msg;
    std::wcout << L" : 에러 : " << msg_buf;
    while (true);
    LocalFree(msg_buf);
}

void read_n_send(CHAR keyInput)
{
    WSABUF wsabuf[1];
    wsabuf[0].buf = reinterpret_cast<CHAR*>(&keyInput);
    wsabuf[0].len = sizeof(CHAR);

    DWORD sent_size;

    //if (wsabuf[0].len == 1)
    //{
    //    bshutdown = true;
    //    return;
    //}

    ZeroMemory(&wsaover, sizeof(wsaover));
    // send_callback 을 통해서 받도록
    int res = WSASend(server_s, wsabuf, 1, nullptr, 0, &wsaover, nullptr);
    if (0 != res)
    {
        print_error("WSASend", WSAGetLastError());
    }
}

void Keep_Recv()
{
    wsabuf.len = BUFSIZE;
    //wsabuf[0].len = BUFSIZE;
    DWORD recv_flag = 0;

    ZeroMemory(&wsaover, sizeof(wsaover));
    int res = WSARecv(server_s, &wsabuf, 1, nullptr, &recv_flag, &wsaover, recv_callback);
    if (res != 0) {
        int err_no = WSAGetLastError();
        if (WSA_IO_PENDING != err_no)		// recv 요청을 걸고 버퍼가 비었을시
            print_error("WSARecv", WSAGetLastError());
    }
}

void CALLBACK recv_Id(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
    player = { server_s, server_a };
    int *id = reinterpret_cast<int*>(wsabuf.buf);
    std::cout << "현재 클라이언트 ID : " << *id << std::endl;
    
    g_players.try_emplace(*id, player);
    
    Keep_Recv();
}

void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
    int p_size = 0;				// 받은 개수만큼
    // (coordinate 구조체 사이즈 8 + 2(사이즈 1 + id 자리)) + id
   
    while (recv_size > p_size)
    {
        int m_size = wsabuf.buf[0 + p_size];   // 첫번째 있는 크기
        int player_num = static_cast<int>(wsabuf.buf[1 + p_size]);
        std::cout << "Player [ " << player_num << " ] :	";  // 두번째 들어있는 플레이어 번호
        char coo[sizeof(Coordinate)];

        for (DWORD i = 0; i < m_size - 2; ++i)
        {
            coo[i] = wsabuf.buf[i + p_size + 2];
        }

        //memcpy(coo, player[0].m_Wsa_buf.buf + 2 + p_size, sizeof(Coordinate) + 1);
        Coordinate* coord = (Coordinate*)(coo);
        if (coord->x == -99 && coord->y == -99)
        {
            if (g_players.find(player_num) != g_players.end())
            {
                g_players.erase(player_num);
                std::cout << player_num << " : 삭제!" << std::endl;
                break;
            }
        }
        std::cout << "X = " << coord->x << ", Y = " << coord->y << std::endl;
        
        // 해당 말이 존재하는지 확인 후 없으면 추가
        if (g_players.find(player_num) == g_players.end())
        {
            ChessPlayer p{ *coord };
            g_players.try_emplace(player_num, p);
        }
        g_players[player_num].Move(*coord);

        p_size = p_size + m_size;
    }
   
    Keep_Recv();
    
}

void CALLBACK send_callback(DWORD err, DWORD sent_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
    //player[0].m_Wsa_buf.len = BUFSIZE;
    /*wsabuf.len = BUFSIZE;
    DWORD recv_flag = 0;
    ZeroMemory(pwsaover, sizeof(*pwsaover));*/

    // recv요청 상태
    // TODO :  콜백 진입시 로직(입력 및 문자열 넘어가면)을 바꾸면 된다.
    //int res = WSARecv(server_s, &wsabuf, 1, nullptr, &recv_flag, pwsaover, recv_callback);
}

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

    server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);

    server_a.sin_family = AF_INET;
    server_a.sin_port = htons(SERVERPORT);

    inet_pton(AF_INET, SERVER_ADDR, &server_a.sin_addr);

    res = connect(server_s, reinterpret_cast<sockaddr*> (&server_a), sizeof(server_a));

    
 

    if (SOCKET_ERROR == res)
    {
        print_error("Connect", WSAGetLastError());
        closesocket(server_s);
        WSACleanup();
    }
    else
    {
        std::cout << "Connect Success with : " << SERVER_ADDR << std::endl;
        // TODO : 일단 본인 클라이언트만 받는거 해결 필요
        
        wsabuf.len = sizeof(int);
        wsabuf.buf = recvBuffer;
        DWORD recv_flag = 0;
        
        ZeroMemory(&wsaover, sizeof(wsaover));
        int res = WSARecv(server_s, &wsabuf, 1, nullptr, &recv_flag, &wsaover, recv_Id);
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
            SleepEx(0, TRUE);
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

    static CImage img;
    static CImage img_background;
    static CImage img_player, img_Enemy;
    static CImage img_Enemy_2;
    static Coordinate coord{0, 0 };
    static RECT rect;


    // 키입력 버퍼
    CHAR keyInput = 0;


    switch (message)
    {
    case WM_CREATE:
	    {
            HRESULT hr = img.Load(TEXT("chess_board.bmp"));
            if (FAILED(hr)) {
                std::cerr << "img Image load failed." << std::endl;
            }
            HRESULT hr_Back = img_background.Load(TEXT("chess_board.bmp"));
            if (FAILED(hr_Back)) {
                std::cerr << "img_background Image load failed." << std::endl;
            }
            HRESULT hr1 = img_player.Load(TEXT("chess_player.png"));
            if (FAILED(hr1)) {
                std::cerr << "player Image load failed." << std::endl;
            }
            HRESULT hr2 = img_Enemy.Load(TEXT("chess_Enemy.png"));
            if (FAILED(hr2)) {
                std::cerr << "Enemy Image load failed." << std::endl;
            }

            HRESULT hr22 = img_Enemy_2.Load(TEXT("chess_Enemy.png"));
            if (FAILED(hr22)) {
                std::cerr << "Enemy2 Image load failed." << std::endl;
            }
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
        
       
	        GetClientRect(hWnd, &rect);

	        HDC hdc = GetDC(hWnd); // 그림 그릴 윈도우의 DC 가져오기
	        HDC hMemdc = CreateCompatibleDC(hdc); // 화면dc(그릴 윈도우) 기반 메모리 DC생성 
	        HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom); // 비트맵 생성
	        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemdc, hBitmap); // 비트맵 선택

            if (img_background != NULL)
                img_background.Draw(hMemdc, 0, 0, rect.right, rect.bottom, 0, 0, img_background.GetWidth(), img_background.GetHeight()); //--- 메모리 DC에 배경 그리기


            for (const auto& p : g_players)
            {
                if (p.second.m_Status == PLAYER && img_player != NULL)
                    img_player.Draw(hMemdc, p.second.m_Coord.x, p.second.m_Coord.y,
                    100, 100, 0, 0, img_player.GetWidth(), img_player.GetHeight());
                else {
                    if (img_Enemy_2 != NULL) {
                        img_Enemy_2.Draw(hMemdc, p.second.m_Coord.x, p.second.m_Coord.y,
                            100, 100, 0, 0, img_Enemy_2.GetWidth(), img_Enemy_2.GetHeight());
                        
                    }
                }
            }

            
            

            BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemdc, 0, 0, SRCCOPY); // 고속복사 hMemdc->hdc

	        SelectObject(hMemdc, hOldBitmap); // 기존 비트맵 선택

            DeleteDC(hMemdc); // 메모리 dc 해제
            DeleteDC(hdc); // 화면 dc 해제
            DeleteObject(hBitmap);
            DeleteObject(hOldBitmap);
        }
        break;

    case WM_KEYDOWN:
		{
        CHAR keyInput;
        //DWORD recv_flag;
        int ret;
    		switch (wParam) {
		    case VK_LEFT:
		    case VK_RIGHT:
		    case VK_UP:
		    case VK_DOWN:

		        keyInput = static_cast<CHAR>(wParam);
                read_n_send(keyInput);
		       
                break;

    		case VK_ESCAPE:
                std::cout << "프로그램 종료" << std::endl;
                closesocket(server_s);
                WSACleanup();
                exit(0);
    		}
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
