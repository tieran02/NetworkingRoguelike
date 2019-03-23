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
	unsigned int m_connectionIdCount{ 1 };
	CircularBuffer<ServerMessage> m_serverMessages{32};

	sf::TcpListener listener;
	sf::SocketSelector selector;
	std::mutex m_acceptMutex;

	//Seed to send to client for level generation
	int m_seed{0};

	//Recieve and send broadcasts
	void sendUdpMessage(MessageType type, char* data, size_t size, sf::IpAddress address, unsigned short port);
	void receiveUDP();


	void setupSocketSelector();
};

