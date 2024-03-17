#pragma once

#include <iostream>

#define SERVERPORT 4000
#define BUFSIZE    512

#include <WS2tcpip.h>	// 최신 고성능 sock
#pragma comment (lib, "WS2_32.LIB")

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
	std::wcout << L" : 에러 : " << msg_buf;
	while (true);
	LocalFree(msg_buf);
}

