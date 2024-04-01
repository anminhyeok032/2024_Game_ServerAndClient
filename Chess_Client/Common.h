#pragma once

#include <iostream>
#include <atlImage.h>
#include <vector>
#include <unordered_map>

#define SERVERPORT 4000
#define BUFSIZE    512


#include <WS2tcpip.h>	// 최신 고성능 sock
#pragma comment (lib, "WS2_32.LIB")

struct Coordinate
{
	int x, y;
};

enum Status
{
	PLAYER,
	ENEMY
};




void print_error(const char* msg, int err_no);
void read_n_send(CHAR keyInput);




