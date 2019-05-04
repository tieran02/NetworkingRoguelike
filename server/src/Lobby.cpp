#include "Lobby.h"
#include "Network.h"
#include "shared/Utility/Log.h"

Lobby::Lobby(Network* network) : m_network(network), m_startGame(false), m_connectedPlayers(0)
{
}

Lobby::~Lobby()
{
}

void Lobby::Create(int maxConnections)
{
	m_maxConnections = maxConnections;
}

void Lobby::Start()
{
	LOG_INFO("Starting Game");
	m_startGame = true;

	//spawn players
	for (auto& connection : m_network->GetConnections())
	{
		m_network->SpawnPlayer(connection.first);
	}
}

bool Lobby::ShouldStart() const
{
	return m_startGame;
}

void Lobby::AddPlayerToLobby()
{
	m_connectedPlayers++;
}

void Lobby::PlayerDisconnected()
{
	m_connectedPlayers--;
}

bool Lobby::IsFull() const
{
	return m_connectedPlayers >= m_maxConnections;
}


void Lobby::Update()
{

}
