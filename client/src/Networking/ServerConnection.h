#pragma once
#include <SFML/Network.hpp>
#include <thread>
#include <shared/Message.h>
#include "shared/CircularBuffer.h"
#include "World.h"

class ServerConnection
{
public:
	ServerConnection(unsigned short port, World* world);
	~ServerConnection();

	void FindServer();
	void Connect();
	void PollMessages();

	bool FoundServer() const;
	bool IsConnected() const;
	void Disconnect();
private:
	World* m_world{ nullptr };
	sf::IpAddress m_serverAddress{sf::IpAddress::None};

	const unsigned short m_broadcastUdpPort;
	sf::IpAddress m_serverIP;
	unsigned short m_serverTcpPort;
	unsigned short m_serverUdpPort;


	sf::TcpSocket m_serverTcpSocket;
	sf::UdpSocket m_serverUdpSocket;
	bool m_isConnected{ false };
	unsigned int m_clientID;

	CircularBuffer<ServerMessage> m_serverMessages{32};

	void sendUdpMessage(MessageType type, char* data, size_t size);
	void sendUdpMessage(const std::string& string);

	void sendTcpMessage(MessageType type, char* data, size_t size);
	void sendTcpMessage(const std::string& string);

	//receive threads
	std::thread updRecieve;
	std::thread tcpRecieve;
	void receiveUDP();
	void receiveTCP();
};

