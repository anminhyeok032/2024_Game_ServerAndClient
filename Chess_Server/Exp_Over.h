#pragma once
#include "stdafx.h"

class EXP_OVER
{
public:
	WSAOVERLAPPED over;
	WSABUF wsabuf[1];
	char buf[BUFSIZE];

	EXP_OVER(int s_id, char* mess, int m_size)
	{
		ZeroMemory(&over, sizeof(over));
		wsabuf[0].buf = buf;
		wsabuf[0].len = m_size + 2;

		buf[0] = m_size + 2;
		buf[1] = s_id;
		memcpy(buf + 2, mess, m_size);
	}
};
