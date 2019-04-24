#pragma once
#include <SFML/Network.hpp>
#include "Connection.h"
#include "shared/MessageQueue.h"
#include <unordered_map>
#include "shared/CircularBuffer.h"
#include "shared/Queue.h"
#include <shared_mutex>
#include "shared/ThreadPool.h"
#include <shared/Messages/NetworkMessages.h>
#include "shared/Collider.h"


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

	void SendSpawnMessage(unsigned int worldID, unsigned int entityID, sf::Vector2f position, sf::Vector2f velocity, unsigned int ownershipID = 0, CollisionLayer layerOverride = CollisionLayer::NONE);
	void SendMovementMessage(unsigned int worldID, sf::Vector2f newPosition, sf::Vector2f velocity);
	void SendHealthMessage(unsigned int worldID, float health, float maxHealth);

	void Connect(Connection* connection);
	void Disconnect(unsigned int connectionID);

	ThreadPool& GetThreadPool() { return m_threadPool; }
private:
	WorldState* m_worldState;
	bool m_running{ false };
	const unsigned short UDP_PORT;
	const unsigned short TCP_PORT;

	ThreadPool m_threadPool{ 32 };

	//server tick rate in milliseconds
	const float TICK_RATE{ (1.0f / 64.0f)  };
	float m_currentTime;

	sf::UdpSocket m_udpSocket;
	std::unordered_map<unsigned int,std::unique_ptr<Connection>> m_connections;
	unsigned int m_connectionIdCount{ 1 };
	Queue<ServerMessage> m_serverMessages;

	std::mutex m_connectionMutex;

	//Recieve and send broadcasts
	void receiveUDP();
	void acceptTCP();

	void sendWorldState();
	void calculateClientPing(unsigned int id, long long clientTimestamp);
};

