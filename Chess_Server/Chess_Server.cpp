#include "Session.h"
#include "Exp_Over.h"


std::unordered_map<int, SESSION> g_players;




void CALLBACK send_callback(DWORD err, DWORD send_size, LPWSAOVERLAPPED pover, DWORD recv_flag)
{
	if (0 != err)
	{
		print_error("WSASend", WSAGetLastError());
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
		g_players.erase(my_id);
		return;
	}

	
	g_players[my_id].check_Coord();
	//g_players[my_id].print_Coord(recv_size);
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
		}
		g_players.try_emplace(id, client_s, id);
		g_players[id++].do_recv();

	}
	closesocket(server_s);
	WSACleanup();
}