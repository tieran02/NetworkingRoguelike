#pragma once
#include <SFML/Network.hpp>
#include "shared/MessageQueue.h"
#include <unordered_map>
#include "shared/CircularBuffer.h"
#include "shared/Queue.h"
#include "shared/ThreadPool.h"
#include <shared/Messages/NetworkMessages.h>
#include "shared/Collider.h"
#include "Lobby.h"

class Connection;
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
	void SendMovementMessage(unsigned int worldID, sf::Vector2f velocity);
	void SendHealthMessage(unsigned int worldID, float health, float maxHealth);

	void Connect(std::unique_ptr<sf::TcpSocket> connection);
	void Disconnect(unsigned int connectionID);
	void SpawnPlayer(unsigned int connectionID);


	float GetCurrentTime() const { return m_currentTime; }
	float GetCurrentTickRate() const { return m_currentTickRate; }

	ThreadPool& GetThreadPool() { return m_threadPool; }
	Queue<ServerMessage>& GetMessageQueue() {return m_serverMessages;}
	std::unordered_map<unsigned int, std::unique_ptr<Connection>>& GetConnections() { return m_connections; }
private:
	Lobby m_lobby;
	WorldState* m_worldState;
	bool m_running{ false };
	const unsigned short UDP_PORT;
	const unsigned short TCP_PORT;

	ThreadPool m_threadPool{ 32 };

	//server tick rate in milliseconds
	const float TICK_RATE{ (1.0f / 32.0f)  };
	float m_currentTime;
	float m_currentTickRate{ 1.0 / 32.0f };
	unsigned int ticksSinceReSync{ 0 };

	sf::UdpSocket m_udpSocket;
	std::unordered_map<unsigned int,std::unique_ptr<Connection>> m_connections;
	unsigned int m_connectionIdCount{ 1 };
	Queue<ServerMessage> m_serverMessages;
	//Client - (0 is to all), protocl to send, message to send, client to ignore in send to all
	std::queue<std::tuple<unsigned int, Protocol, Message, unsigned int>> m_messagesToSend;

	std::mutex m_connectionMutex;

	void pollMessages();
	//Recieve and send broadcasts
	void receiveUDP();
	void acceptTCP();

    bool m_close{false};
	std::thread m_receiveUdpThread;
    std::thread m_receiveTcpThread;

	void sendWorldState();
	void calculateClientPing(unsigned int id, long long clientTimestamp);
};

