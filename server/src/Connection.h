#pragma once
#include <SFML/Network.hpp>
#include <memory>

class Network;

class Connection
{
public:
	Connection();
	~Connection();

	void Connect(std::unique_ptr<sf::TcpSocket>& tcpSocket, unsigned int id);
	void Disconnect();

	sf::TcpSocket& GetTcpSocket() const { return *m_tcpSocket; }
	const sf::IpAddress& GetAddress() const { return m_address; }
	const unsigned short& GetPort() const { return m_port; }
	bool IsConnected() const { return m_isConnected; }
	unsigned int GetConnectionID() const { return m_connectionID; }
private:
	unsigned int m_connectionID;
	sf::IpAddress m_address;
	unsigned short m_port;
	std::unique_ptr<sf::TcpSocket> m_tcpSocket;
	bool m_isConnected{ false };
};

