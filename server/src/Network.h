#pragma once
#include <SFML/Network.hpp>
#include "Connection.h"
#include <queue>

struct Message
{
	enum class Protocol
	{
		UPD,
		TCP
	};


	Message(Protocol proto)
	{
		protocol = proto;
	}

	Message(Protocol proto, const char* data, size_t size, sf::IpAddress ip, unsigned short port)
	{
		protocol = proto;
		message = data;
		this->size = size;
		senderAddress = ip;
		senderPort = port;
	}


	Protocol protocol;
	const char* message;
	size_t size;
	sf::IpAddress senderAddress;
	unsigned short senderPort;
};

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

