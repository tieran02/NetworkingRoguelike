#pragma once
#include "Networking/Message.h"
#include <SFML/Network.hpp>
#include <queue>
#include <thread>

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
private:
	sf::IpAddress m_serverAddress{sf::IpAddress::None};
	const unsigned short m_serverUdpPort;
	const unsigned short m_serverTcpPort;

	sf::TcpSocket m_serverTcpSocket;
	sf::UdpSocket m_serverUdpSocket;
	bool m_isConnected{ false };
	unsigned int clientID;

	std::queue<ServerMessage> m_receivedMessages;

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

