#pragma once
#include <SFML/Network.hpp>
#include <memory>

class Network;

class Connection
{
public:
	Connection();
	~Connection();

	void Connect(std::unique_ptr<sf::TcpSocket>& tcpSocket);
	void Disconnect();

	sf::TcpSocket& GetTcpSocket() const { return *m_tcpSocket; }
	const sf::IpAddress& GetAddress() const { return m_address; }
	const unsigned short& GetPort() const { return m_port; }
	bool IsConnected() const { return m_isConnected; }
private:
	sf::IpAddress m_address;
	unsigned short m_port;
	std::unique_ptr<sf::TcpSocket> m_tcpSocket;
	bool m_isConnected{ false };
};

