#include "stdafx.h"

constexpr short PORT = 4000;
constexpr int BUFSIZE = 256;


int main()
{
	std::wcout.imbue(std::locale("korean"));

	int res;
	Coordinate coord{0,0};

	// TODO 윈도우 창 크기 맞춰서 자동 rect 크기 조정되게 하는 법 물어보기
	static RECT rect {0, 0, 784, 741};

	WSADATA WSAData;
	res = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (0 != res)
	{
		print_error("WSAStartup", WSAGetLastError());
	}

	SOCKET server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
	SOCKADDR_IN server_a;
	server_a.sin_family = AF_INET;
	server_a.sin_port = htons(PORT);
	server_a.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(server_s, reinterpret_cast<sockaddr*>(&server_a), sizeof(server_a));
	if (0 != res)
	{
		print_error("bind", WSAGetLastError());
	}

	res = listen(server_s, SOMAXCONN);
	if (0 != res)
	{
		print_error("listen", WSAGetLastError());
	}

	int addr_size = sizeof(server_a);
	SOCKET client_s = WSAAccept(server_s, reinterpret_cast<sockaddr*> (&server_a), &addr_size, nullptr, 0);
	if (client_s != INVALID_SOCKET)
	{
		std::cout << "클라이언트 연결 성공" << std::endl;
	}

	static int xPos = 0, yPos = 0;

	while (true)
	{
		char buf[BUFSIZE];
		WSABUF wsabuf[1];

		wsabuf[0].buf = buf;
		wsabuf[0].len = BUFSIZE;
		DWORD recv_size;
		DWORD recv_flag = 0;

		res = WSARecv(client_s, wsabuf, 1, &recv_size, &recv_flag, nullptr, nullptr);
		if (0 != res)
		{
			print_error("WSARecv", WSAGetLastError());
			closesocket(server_s);
			WSACleanup();
		}

		CHAR keyInput = static_cast<CHAR>(*wsabuf[0].buf);

		std::cout << "Client Sent : ";
		switch (buf[0]) {
		case VK_LEFT:
			std::cout << "VK_LEFT";
			xPos -= rect.right / 8;
			break;
		case VK_RIGHT:
			std::cout << "VK_RIGHT";
			xPos += rect.right / 8;
			break;
		case VK_UP:
			std::cout << "VK_UP";
			yPos -= rect.bottom / 8;
			break;
		case VK_DOWN:
			std::cout << "VK_DOWN";
			yPos += rect.bottom / 8;
			break;
		}
		std::cout << std::endl;

		xPos = max(0, min(xPos, rect.right - 100));
		yPos = max(0, min(yPos, rect.bottom - 100));

		coord.x = xPos;
		coord.y = yPos;

		char buffer[sizeof(Coordinate)];
		memcpy(buffer, &coord, sizeof(Coordinate));

		wsabuf[0].buf = buffer;
		wsabuf[0].len = sizeof(Coordinate);
		DWORD sent_size;

		res = WSASend(client_s, wsabuf, 1, &sent_size, 0, nullptr, nullptr);
		if (0 != res)
		{
			print_error("WSASend", WSAGetLastError());
			closesocket(server_s);
			WSACleanup();
		}
		std::cout << "Send Coordiates X, Y : " << coord.x << ", "<< coord.y << std::endl;
	}
	closesocket(server_s);
	closesocket(client_s);
	WSACleanup();
}