#include "Connection.h"
#include <iostream>


Connection::Connection()
{
}

Connection::~Connection()
{
}

void Connection::Connect(std::unique_ptr<sf::TcpSocket>& tcpSocket, unsigned int id)
{
	if(m_isConnected)
	{
		std::cout << "Client @" << m_address << ":" << m_port << " is already connected!" << std::endl;
		return;
	}

	m_tcpSocket = std::move(tcpSocket);
	m_address = m_tcpSocket->getRemoteAddress();
	m_port = m_tcpSocket->getRemotePort();
	m_isConnected = true;
	m_connectionID = id;

	std::cout << "Client Connected @" << m_address << ":" << m_port << std::endl;

}

void Connection::Disconnect()
{
	m_tcpSocket->disconnect();
	m_isConnected = false;
	std::cout << "Client Disconnected @" << m_address << ":" << m_port << std::endl;

}
