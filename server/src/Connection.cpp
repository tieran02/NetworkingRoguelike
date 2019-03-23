#include "Connection.h"
#include <iostream>
#include "shared/ConnectionMessage.h"


Connection::Connection()
{
}

Connection::~Connection()
{
}

void Connection::Connect(std::unique_ptr<sf::TcpSocket>& tcpSocket, unsigned int id, int seed)
{
	if(m_isConnected)
	{
		std::cout << "Client @" << m_address << ":" << m_portTCP << " is already connected!" << std::endl;
		return;
	}

	m_tcpSocket = std::move(tcpSocket);
	m_address = m_tcpSocket->getRemoteAddress();
	m_portTCP = m_tcpSocket->getRemotePort();
	m_isConnected = true;
	m_connectionID = id;

	//Send client connection data
	const ConnectionMessage message(id, seed, m_portTCP+1);
	SendTCP(message);

	//bind udp for this client
	m_udpSocket = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket());
	if (m_udpSocket->bind(m_portTCP + 1) != sf::Socket::Done)
	{
		std::cerr << "failed to bind upd socket\n";
	}

	std::cout << "Client Connected @" << m_address << ":" << m_portTCP << std::endl;

}

void Connection::Disconnect()
{
	m_receiveThread->detach();
	m_tcpSocket->disconnect();
	m_isConnected = false;
	std::cout << "Client Disconnected @" << m_address << ":" << m_portTCP << std::endl;

}

void Connection::ReceiveTCP(CircularBuffer<ServerMessage>& messageBuffer)
{
	std::thread([&messageBuffer, this]
	{
		size_t received;
		const size_t maxMessageSize = 256;
		char buffer[maxMessageSize];

		auto data = m_tcpSocket->receive(buffer, maxMessageSize, received);
		if (data != sf::Socket::Done)
		{
			if (data == sf::Socket::Disconnected)
			{
				//Disconnect();
				return;;
			}
			std::cerr << "Failed To receive tcp packet\n";
		}
		const Message message{ buffer };

		ServerMessage serverMessage(message);
		serverMessage.protocol = Protocol::TCP;
		serverMessage.senderAddress = m_address;
		serverMessage.senderPort = m_portTCP;
		messageBuffer.enqueue(serverMessage);
	}).detach();
}

void Connection::ReceiveUDP(CircularBuffer<ServerMessage>& messageBuffer)
{
	std::thread([&messageBuffer, this]
	{
		sf::IpAddress sender;
		unsigned short port;
		size_t received;
		const size_t maxMessageSize = 256;
		char buffer[maxMessageSize];

		if (m_udpSocket->receive(buffer, maxMessageSize, received, sender, port) != sf::Socket::Done)
		{
			std::cerr << "Failed To receive udp packet\n";
		}

		const Message message{ buffer };

		ServerMessage serverMessage(message);
		serverMessage.protocol = Protocol::UPD;
		serverMessage.senderAddress = sender;
		serverMessage.senderPort = port;
		messageBuffer.enqueue(serverMessage);
	});
}

void Connection::SendTCP(const Message& msg) const
{
	auto buffer = msg.GetBuffer();
	if (m_tcpSocket->send(buffer.data(), buffer.size()) != sf::Socket::Done)
	{
		std::cerr << "Failed To send message over TCP to client: " << m_connectionID << std::endl;;
	}
}

void Connection::SendUDP(const Message& msg) const
{
	auto buffer = msg.GetBuffer();
	if(m_udpSocket->send(buffer.data(), buffer.size(),m_address, m_portUDP) != sf::Socket::Done)
	{
		std::cerr << "Failed To send message over UDP to client: " << m_connectionID << std::endl;;
	}
}
