#pragma once
#include <SFML/Network.hpp>
#include <thread>
#include <shared/Messages/NetworkMessages.h>
#include "shared/CircularBuffer.h"
#include "World.h"
#include "shared/Queue.h"

class ServerConnection
{
public:
	ServerConnection(unsigned short port, World* world);
	~ServerConnection();

	void FindServer();
	void Connect();
	void UpdateTick();
	void PollMessages();

	bool FoundServer() const;
	bool IsConnected() const;
	void Disconnect();

    void SendUdpMessage(const Message& message);
    void SendTcpMessage(const Message& message);
	float GetPing() const { return  m_ping; }

	void NotifyWorldGeneration();

	unsigned int GetColientID() const { return m_clientID; }

	void SendEntityStateMessage(const Entity& entity);
	void SendMovementMessage(unsigned int worldID, sf::Vector2f newPosition, sf::Vector2f velocity);
	void SendSpawnRequestMessage(const std::string& entityName, sf::Vector2f position, sf::Vector2f velocity);
	void SendProjectileRequestMessage(const std::string& entityName, sf::Vector2f position, sf::Vector2f velocity, CollisionLayer side);
	void SendEntityDestroyMessage(unsigned int worldID);
	void SendHealthMessage(unsigned int worldID, float health, float maxHealth);


	sf::Time TimeSinceLastMessage() const;
private:
	//server tick rate in milliseconds
	const float TICK_RATE{ 1.0f/64.0f };
	sf::Time m_lastTick;
	sf::Clock m_tickClock;
	float m_ping;
	sf::Clock m_timeSinceLastMessage;

	World* m_world{ nullptr };
	sf::IpAddress m_serverAddress{sf::IpAddress::None};

	const unsigned short m_broadcastUdpPort;
	sf::IpAddress m_serverIP;
	unsigned short m_serverTcpPort;


	sf::TcpSocket m_serverTcpSocket;
	sf::UdpSocket m_serverUdpSocket;
	bool m_isConnected{ false };
	unsigned int m_clientID;

	Queue<ServerMessage> m_serverMessages;

	//receive threads
	std::thread updRecieve;
	std::thread tcpRecieve;
	void receiveUDP();
	void receiveTCP();

	void sendEntityStates();
	void updateEntityNetworkState(unsigned int worldID, sf::Vector2f newPosition, sf::Vector2f velocity);
	void calculatePing(long long  serverTimestamp);

};

