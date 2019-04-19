#pragma once
#include <SFML/Network.hpp>
#include <memory>
#include <thread>
#include "shared/Message.h"
#include "shared/Queue.h"

class WorldState;
class Network;

class Connection
{
	friend WorldState;
	friend Network;
public:
	Connection(Network* network);
	~Connection();
	Connection(const Connection& other) = delete;
	Connection& operator=(const Connection& other) = delete;

	void Connect(unsigned int id, int seed, Queue<ServerMessage>& messageBuffer);
	void Disconnect();

	sf::TcpSocket* GetTcpSocket() const { return m_tcpSocket.get(); }
	sf::UdpSocket* GetUdpSocket() const { return m_udpSocket.get(); }
	const sf::IpAddress& GetAddress() const { return m_address; }
	const unsigned short& GetPort() const { return m_portTCP; }
	bool IsConnected() const { return m_isConnected; }
	bool IsSetup() const { return m_isSetup; }

	unsigned int GetConnectionID() const { return m_connectionID; }

	//Receive TCP message (creates a new thread)
	void ReceiveTCP(Queue<ServerMessage>& messageBuffer);

	void SendTCP(const Message& msg) const;
	void SendUDP(const Message& msg) const;
private:
	unsigned int m_connectionID;
	sf::IpAddress m_address;
	unsigned short m_portTCP;
	unsigned short m_portUDP{0};
	unsigned int m_entityID{0};
	float m_ping;

	std::unique_ptr<sf::TcpSocket> m_tcpSocket;
	std::unique_ptr<sf::UdpSocket> m_udpSocket;
	bool m_isConnected{ false };
	bool m_isSetup{ false };
	std::mutex m_setupMutex;
	std::condition_variable m_cv;

	Network* m_network;
	std::thread receiveThread;
};

