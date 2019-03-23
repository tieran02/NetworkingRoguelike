#pragma once
#include <SFML/Network.hpp>
#include <memory>
#include <thread>
#include "shared/Message.h"
#include "shared/CircularBuffer.h"

class Network;

class Connection
{
public:
	Connection();
	~Connection();

	void Connect(std::unique_ptr<sf::TcpSocket>& tcpSocket, unsigned int id, int seed);
	void Disconnect();

	sf::TcpSocket& GetTcpSocket() const { return *m_tcpSocket; }
	const sf::IpAddress& GetAddress() const { return m_address; }
	const unsigned short& GetPort() const { return m_portTCP; }
	bool IsConnected() const { return m_isConnected; }
	unsigned int GetConnectionID() const { return m_connectionID; }

	//Receive TCP message (creates a new thread)
	void ReceiveTCP(CircularBuffer<ServerMessage>& messageBuffer);
	void ReceiveUDP(CircularBuffer<ServerMessage>& messageBuffer);

	void SendTCP(const Message& msg) const;
	void SendUDP(const Message& msg) const;
private:
	unsigned int m_connectionID;
	sf::IpAddress m_address;
	unsigned short m_portTCP;
	unsigned short m_portUDP;

	std::unique_ptr<sf::TcpSocket> m_tcpSocket;
	std::unique_ptr<sf::UdpSocket> m_udpSocket;
	bool m_isConnected{ false };
	std::shared_ptr<std::thread> m_receiveThread;
};

