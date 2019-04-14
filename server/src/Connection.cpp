#include "Connection.h"
#include <iostream>
#include "shared/ConnectionMessage.h"
#include "WorldState.h"
#include <sstream>
#include "shared/Utility/Log.h"
#include "Network.h"


Connection::Connection(Network* network): m_connectionID(0), m_portTCP(0)
{
	m_network = network;

	m_tcpSocket = std::unique_ptr<sf::TcpSocket>(new sf::TcpSocket());
	m_udpSocket = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket());
}

Connection::~Connection()
{
}

void Connection::Connect(unsigned int id, int seed)
{
	if(m_isConnected)
	{
		std::stringstream stream;
		stream << "Client @" << m_address << ":" << m_portTCP << " is already connected!" ;
		LOG_INFO(stream.str());
		return;
	}

	m_address = m_tcpSocket->getRemoteAddress();
	m_portTCP = m_tcpSocket->getRemotePort();
	m_isConnected = true;
	m_connectionID = id;

	//Send client connection data
	const ConnectionMessage message(id, seed, m_tcpSocket->getLocalPort()+1); //send the local tcp port plus one for the udp port
	SendTCP(message);

	//bind udp for this client
	m_udpSocket = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket());


	std::stringstream stream;
	stream << "Client Connected @" << m_address << ":" << m_portTCP ;
	LOG_INFO(stream.str());
}

void Connection::Disconnect()
{
	m_tcpSocket->disconnect();
	m_isConnected = false;


	std::stringstream stream;
	stream << "Client Disconnected @" << m_address << ":" << m_portTCP ;
	LOG_INFO(stream.str());

}

void Connection::ReceiveTCP(Queue<ServerMessage>& messageBuffer)
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
		LOG_ERROR("Failed To receive tcp packet");
	}
	else
	{
		std::stringstream stream;
		stream << "Recieved TCP message from client:" << m_connectionID ;
		LOG_TRACE(stream.str());
	}
	const Message message{ buffer };

	ServerMessage serverMessage(message);
	serverMessage.protocol = Protocol::TCP;
	serverMessage.senderAddress = m_address;
	serverMessage.senderPort = m_portTCP;
	messageBuffer.enqueue(serverMessage);
}

void Connection::ReceiveUDP(Queue<ServerMessage>& messageBuffer)
{
	sf::IpAddress sender;
	unsigned short port;
	size_t received;
	const size_t maxMessageSize = 256;
	char buffer[maxMessageSize];

	if (m_udpSocket->receive(buffer, maxMessageSize, received, sender, port) != sf::Socket::Done)
	{
		LOG_ERROR("Failed To receive udp packet");
	}
	else
	{
		std::stringstream stream;
		stream << "Recieved UDP message from client:" << m_connectionID ;
		LOG_TRACE(stream.str());
	}

	//check if client udp port has been set
	if(m_portUDP == 0)
	{
		m_portUDP = port;
	}

	const Message message{ buffer };

	ServerMessage serverMessage(message);
	serverMessage.protocol = Protocol::UPD;
	serverMessage.senderAddress = sender;
	serverMessage.senderPort = port;
	messageBuffer.enqueue(serverMessage);
}

void Connection::SendTCP(const Message& msg) const
{
	auto buffer = msg.GetBuffer();
	if (m_tcpSocket->send(buffer.data(), buffer.size()) != sf::Socket::Done)
	{
		std::stringstream stream;
		stream << "Failed To send message over TCP to client: " << m_connectionID ;
		LOG_ERROR(stream.str());
	}
}

void Connection::SendUDP(const Message& msg) const
{
	auto buffer = msg.GetBuffer();
	if(m_udpSocket->send(buffer.data(), buffer.size(),m_address, m_portUDP) != sf::Socket::Done)
	{
		std::stringstream stream;
		stream << "Failed To send message over UDP to client: " << m_connectionID ;
		LOG_ERROR(stream.str());
	}
}
