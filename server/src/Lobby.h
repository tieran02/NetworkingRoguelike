#pragma once
#include <unordered_map>

class Network;

class Lobby
{
public:
	Lobby(Network* network);
	~Lobby();

	void Create(int maxConnections);
	void Update();

	bool ShouldStart() const;

	void AddPlayerToLobby();
	void PlayerDisconnected();
	bool IsFull() const;

private:
	Network* m_network;
	bool m_startGame{ false };
	int m_maxConnections;
	int m_connectedPlayers;

	void startGame();

};