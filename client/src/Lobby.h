#pragma once
#include "Networking/ServerConnection.h"

class Lobby
{
public:
	Lobby(ServerConnection& connection);
	~Lobby();

private:
	ServerConnection& m_connection;
	bool m_startGame{ false };
};

