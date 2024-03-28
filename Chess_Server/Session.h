#pragma once
#include "stdafx.h"
#include "Exp_Over.h"


class SESSION {

	char buf[BUFSIZE];
	WSABUF wsabuf[1];
	SOCKET client_s;
	WSAOVERLAPPED over;
	int xPos, yPos;
	Coordinate coord{ 0,0 };

public:
	SESSION(SOCKET s, int my_id) : client_s(s)
	{
		g_session_map[&over] = my_id;
		wsabuf[0].buf = buf;
		wsabuf[0].len = BUFSIZE;
		xPos = yPos = 0;
	}
	SESSION() {
		std::cout << "ERROR";
		exit(-1);
	}
	~SESSION() { closesocket(client_s); }

	void do_recv()
	{
		DWORD recv_flag = 0;
		ZeroMemory(&over, sizeof(over));
		wsabuf[0].buf = buf;
		wsabuf[0].len = BUFSIZE;
		
		int res = WSARecv(client_s, wsabuf, 1, nullptr, &recv_flag, &over, recv_callback);
		if (0 != res)
		{
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)		// recv 요청을 걸고 버퍼가 비었을시
				print_error("WSARecv", WSAGetLastError());
		}
	}

	void do_send(int s_id, char* mess, int recv_size)
	{
		auto b = new EXP_OVER(s_id, mess, recv_size);
		int res = WSASend(client_s, b->wsabuf, 1, nullptr, 0, &b->over, send_callback);
		if (0 != res)
		{
			print_error("WSASend", WSAGetLastError());
		}
	}

	void print_Coord(DWORD recv_size)
	{
		int my_id = g_session_map[&over];
		std::cout << "Client [ " << my_id << " ] Sent: ";
		for (DWORD i = 0; i < recv_size; ++i)
		{
			std::cout << buf[i];
		}
		std::cout << std::endl;
	}

	void check_Coord()
	{
		int my_id = g_session_map[&over];
		std::cout << "Client [ " << my_id << " ] Sent: ";
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

		std::cout << "[" << my_id  << "]" << "Send Coordiates X, Y : " << coord.x << ", " << coord.y << std::endl;
	}

	void broadcast(int m_size, std::unordered_map<int, SESSION> &g_players)
	{
		for (auto& p : g_players)
			p.second.do_send(g_session_map[&over], wsabuf[0].buf, m_size);
			
	}

};