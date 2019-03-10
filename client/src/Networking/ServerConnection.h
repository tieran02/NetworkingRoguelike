#pragma once
#include <SFML/Network.hpp>
#include <thread>
#include <shared/Message.h>
#include "shared/CircularBuffer.h"

class ServerConnection
{
public:
	ServerConnection(unsigned short port);
	~ServerConnection();

	void FindServer();
	void Connect();
	void PollMessages();

	bool FoundServer() const;
	bool IsConnected() const;
	void Disconnect();
	int GetSeed() const { return m_seed; }
private:
	sf::IpAddress m_serverAddress{sf::IpAddress::None};
	const unsigned short m_serverUdpPort;
	const unsigned short m_serverTcpPort;

	sf::TcpSocket m_serverTcpSocket;
	sf::UdpSocket m_serverUdpSocket;
	bool m_isConnected{ false };
	unsigned int m_clientID;
	unsigned int m_seed;

	CircularBuffer<ServerMessage> m_serverMessages{32};

	void sendUdpMessage(MessageType type, char* data, size_t size, sf::IpAddress address, unsigned short port);
	void sendUdpMessage(const std::string& string, sf::IpAddress address, unsigned short port);

	void sendTcpMessage(MessageType type, char* data, size_t size);
	void sendTcpMessage(const std::string& string);

	//receive threads
	std::thread updRecieve;
	std::thread tcpRecieve;
	void receiveUDP();
	void receiveTCP();
};

