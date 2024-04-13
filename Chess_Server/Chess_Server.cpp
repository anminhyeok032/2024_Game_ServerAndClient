#include "Session.h"
#include "Exp_Over.h"


std::unordered_map<int, SESSION> g_players;
char buf[BUFSIZE];
char buf1[BUFSIZE];
WSABUF wsabuf, wsabuf1;



void CALLBACK send_callback(DWORD err, DWORD send_size, LPWSAOVERLAPPED pover, DWORD recv_flag)
{
	if (0 != err)
	{
		int my_id = g_session_map[pover];
		print_error("WSASend", WSAGetLastError());

		std::cout << "[" << my_id << "]" << "의 접속이 끊어짐" << std::endl;

		Coordinate coord{ -99, -99 };
		memcpy(buf, &coord, sizeof(Coordinate));

		wsabuf.buf = buf;
		wsabuf.len = sizeof(Coordinate);

		for (auto& p : g_players) {
			p.second.do_send(my_id, wsabuf.buf, wsabuf.len);
		}

		g_players.erase(my_id);
	}

	auto b = reinterpret_cast<EXP_OVER*>(pover);		// 해당 메모리 해제
	delete b;
}


void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pover, DWORD recv_flag)
{
	int my_id = g_session_map[pover];
	if (0 != err || recv_size == 0)
	{
		std::cout << "[" << my_id << "]" << "의 접속이 끊어짐" << std::endl;

		Coordinate coord{ -99, -99 };
		memcpy(buf, &coord, sizeof(Coordinate));

		wsabuf.buf = buf;
		wsabuf.len = sizeof(Coordinate);

		for (auto& p : g_players) {
			p.second.do_send(my_id, wsabuf.buf, wsabuf.len);
		}

		g_players.erase(my_id);
		
		return;
	}

	
	g_players[my_id].check_Coord();
	g_players[my_id].broadcast(recv_size, g_players);
	g_players[my_id].do_recv();
}



int main()
{
	std::wcout.imbue(std::locale("korean"));

	int res;
	Coordinate coord{0,0};



	WSADATA WSAData;
	res = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (0 != res)
	{
		print_error("WSAStartup", WSAGetLastError());
	}

	SOCKET server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
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


	static int xPos = 0, yPos = 0;
	int id = 0;

	while (true)
	{
		std::cout << "[" << id << "] " << "Accept 진입" << std::endl;
		SOCKET client_s = WSAAccept(server_s, reinterpret_cast<sockaddr*> (&server_a), &addr_size, nullptr, 0);
		if (client_s != INVALID_SOCKET)
		{
			std::cout << "클라이언트 [ " << id << " ] 연결 성공" << std::endl;

			// 자신 번호 알려주기
			WSABUF wsabuf;
			char buf[sizeof(int)];
			memcpy(buf, &id, sizeof(int));
			wsabuf.buf = buf;
			wsabuf.len = sizeof(id);
			WSAOVERLAPPED over;
			ZeroMemory(&over, sizeof(over));
			DWORD recv_flag = 0;
			int res = WSASend(client_s, &wsabuf, 1, nullptr, recv_flag, &over, nullptr);

			/*int* idPtr = reinterpret_cast<int*>(wsabuf.buf);
			int idValue = *idPtr;
			std::cout << idValue << std::endl;*/

			if (0 != res)
			{
				print_error("WSASend - ID", WSAGetLastError());
			}
		}
		g_players.try_emplace(id, client_s, id);
		
		// TODO: 접속시 broadcast로 모두의 좌표 모두에게 한번씩 쏴주기
		Coordinate coord{ 0,0 };
		memcpy(buf, &coord, sizeof(Coordinate));

		wsabuf.buf = buf;
		wsabuf.len = sizeof(Coordinate);

		for (auto& p : g_players) {
			p.second.do_send(id, wsabuf.buf, wsabuf.len);
		}

		for (auto& p : g_players) {
			Coordinate coord{ 0,0 };
			coord = p.second.GetCoord();
			memcpy(buf1, &coord, sizeof(Coordinate));

			wsabuf1.buf = buf1;
			wsabuf1.len = sizeof(Coordinate);
			if (id != p.first)
				g_players[id].do_send(p.first, wsabuf1.buf, wsabuf1.len);
		}

		/*for (auto& i : g_players) {
			Coordinate coord = i.second.GetCoord();
			memcpy(buf, &coord, sizeof(Coordinate));
			wsabuf.buf = buf;
			wsabuf.len = sizeof(Coordinate);
			for (auto& p : g_players) {
				p.second.do_send(i.first, wsabuf.buf, wsabuf.len);
			}
		}*/


		g_players[id++].do_recv();

	}
	closesocket(server_s);
	WSACleanup();
}