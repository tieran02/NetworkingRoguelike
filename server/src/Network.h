#pragma once
#include <SFML/Network.hpp>
#include "Connection.h"
#include <queue>
#include "Message.h"

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

	std::vector<std::unique_ptr<Connection>> m_connections;
	std::queue<Message> m_retrievedMessages;

	void acceptTCP();
	void retrieveUDP();
	void retrieveTCP();
};

