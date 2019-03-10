#pragma once
#include <SFML/Network.hpp>
#include "Connection.h"
#include <queue>
#include "shared/MessageQueue.h"
#include <unordered_map>
#include "shared/CircularBuffer.h"

class Network
{
public:
	Network(unsigned short port);
	~Network();

	void Start();
private:
	bool m_running{ false };
	const unsigned short UDP_PORT;
	const unsigned short TCP_PORT;

	sf::UdpSocket m_udpSocket;
	std::unordered_map<unsigned int,std::unique_ptr<Connection>> m_connections;
	unsigned int m_connectionIdCount{ 0 };
	CircularBuffer<ServerMessage> m_serverMessages{32};

	//Seed to send to client for level generation
	int m_seed{0};

	void sendUdpMessage(MessageType type, char* data, size_t size, sf::IpAddress address, unsigned short port);
	void sendUdpMessage(const std::string& string, sf::IpAddress address, unsigned short port);

	void sendTcpMessage(MessageType type, char* data, size_t size, int clientID);
	void sendTcpMessage(const std::string& string, int clientID);
	void sendTcpMessage(Message* message, int clientID);

	void acceptTCP();
	void receiveUDP();
	void receiveTCP();
};

