#pragma once
#include "Common.h"

class ChessPlayer
{
public:
	WSABUF m_Wsa_buf;
	SOCKET m_Socket;
	SOCKADDR_IN m_Socket_Addr;
	Coordinate m_Coord{ 0, 0 };

	// TODO : 플레이어 캐릭터에 대한 enum 생성하여 다른 npc 사진 구분하기

	Status m_Status;

public:
	ChessPlayer(){}

	ChessPlayer(SOCKET socket, SOCKADDR_IN sock_addr) : m_Socket(socket), m_Socket_Addr(sock_addr)
	{
		m_Status = PLAYER;

	}

	ChessPlayer(Coordinate coord) : m_Coord(coord)
	{
		m_Socket = INVALID_SOCKET;
		m_Status = ENEMY;

	}

	~ChessPlayer()
	{
		//closesocket(m_Socket);
	}

	void Move(Coordinate coord);
	Coordinate GetCoord() { return  m_Coord; };

	
};