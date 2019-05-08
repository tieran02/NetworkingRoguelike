#pragma once
#include <SFML/Network.hpp>
#include <thread>
#include <shared/Messages/NetworkMessages.h>
#include "shared/CircularBuffer.h"
#include "World.h"
#include "shared/Queue.h"
#include <string>

class ChatBox;
class ServerConnection
{
public:
	ServerConnection(unsigned short port, World* world, const std::string& playerName);
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

	unsigned int GetClientID() const { return m_clientID; }

	void SendEntityStateMessage(const Entity& entity);
	void SendMovementMessage(unsigned int worldID, sf::Vector2f velocity);
	void SendSpawnRequestMessage(const std::string& entityName, sf::Vector2f position, sf::Vector2f velocity);
	void SendProjectileRequestMessage(const std::string& entityName, sf::Vector2f position, sf::Vector2f velocity, CollisionLayer side);
	void SendProjectileRequestMessages(std::vector< std::tuple<sf::Vector2f, sf::Vector2f, CollisionLayer>> projectiles);
	void SendEntityDestroyMessage(unsigned int worldID);
	void SendHealthMessage(unsigned int worldID, float health, float maxHealth);

	void SendChatMessage(const std::string& message);

	const std::string& GetPlayerName() { return m_clientName; }
	const std::unordered_map<unsigned int, std::string>& GetPlayerNames() { return m_connectedClientNames; }
	bool InProgress() const { return m_gameInProgress; }

	ChatBox& GetChatBox() { return m_chatBox; }
private:
	float m_ping;

	ChatBox m_chatBox;
	World* m_world{ nullptr };
	sf::IpAddress m_serverAddress{sf::IpAddress::None};

	const unsigned short m_broadcastUdpPort;
	sf::IpAddress m_serverIP;
	unsigned short m_serverTcpPort;
	std::string m_clientName;
	std::unordered_map<unsigned int,std::string> m_connectedClientNames;

	sf::TcpSocket m_serverTcpSocket;
	sf::UdpSocket m_serverUdpSocket;
	bool m_isConnected{ false };
	unsigned int m_clientID;
	bool m_gameInProgress{ false };

	Queue<ServerMessage> m_serverMessages;
	//receive threads
	bool m_close{false};
	std::thread m_receiveUdpThread;
    std::thread m_receiveTcpThread;

	void receiveUDP();
	void receiveTCP();

	void sendEntityStates();
	void updateEntityVelocityFromServer(unsigned int worldID, sf::Vector2f velocity);
	void updateEntityPositionFromServer(unsigned int worldID, sf::Vector2f newPosition, sf::Vector2f velocity);

	void calculatePing(long long  serverTimestamp);

};