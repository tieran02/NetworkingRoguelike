#pragma once
#include <SFML/Network.hpp>
#include "Connection.h"
#include "shared/MessageQueue.h"
#include <unordered_map>
#include "shared/CircularBuffer.h"
#include "shared/Queue.h"
#include <shared_mutex>

class WorldState;
class Network
{
public:
	Network(WorldState& world, unsigned short port);
	~Network();

	void Start();

	void SendUdpMessage(const Message& message, sf::IpAddress address, unsigned short port);
	void SendToAllUDP(const Message& message, unsigned int ignore = 0);
	void SendToAllTCP(const Message& message, unsigned int ignore = 0);

	void SendSpawnMessage(unsigned int worldID, unsigned int entityID, sf::Vector2f position, sf::Vector2f velocity, unsigned int ownershipID = 0);
	void SendMovementMessage(unsigned int worldID, sf::Vector2f newPosition, sf::Vector2f velocity);

	void Connect(Connection* connection);
	void Disconnect(unsigned int connectionID);

private:
	WorldState* m_worldState;
	bool m_running{ false };
	const unsigned short UDP_PORT;
	const unsigned short TCP_PORT;

	//server tick rate in milliseconds
	const float TICK_RATE{ 1.0f / 64.0f };
	sf::Time m_lastTick;
	sf::Clock m_tickClock;

	sf::UdpSocket m_udpSocket;
	std::unordered_map<unsigned int,std::unique_ptr<Connection>> m_connections;
	unsigned int m_connectionIdCount{ 1 };
	Queue<ServerMessage> m_serverMessages;

	std::mutex m_connectionMutex;

	//Recieve and send broadcasts
	void receiveUDP();
	void acceptTCP();

	void sendWorldState();
};

