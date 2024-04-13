#pragma once

#include <iostream>
#include <WS2tcpip.h>	// �ֽ� ���� sock
#include <unordered_map>
#pragma comment (lib, "WS2_32.LIB")

void CALLBACK send_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK recv_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

// ������ �����Ϳ� ���̵� ���� unordered_map
std::unordered_map<LPWSAOVERLAPPED, int> g_session_map;

constexpr short PORT = 4000;
constexpr int BUFSIZE = 256;

static RECT rect {0, 0, 784, 741};

struct Coordinate
{
	int x, y;
};

void print_error(const char* msg, int err_no)
{
	WCHAR* msg_buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
	std::cout << msg;
	std::wcout << L" : ���� : " << msg_buf;
	while (true);
	LocalFree(msg_buf);
}


