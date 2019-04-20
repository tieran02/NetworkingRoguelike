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

void Connection::Connect(unsigned int id, int seed, Queue<ServerMessage>& messageBuffer)
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

	//start TCP receive thread
	receiveThread = std::thread(&Connection::ReceiveTCP, this, std::ref(messageBuffer));

	std::stringstream stream;
	stream << "Client Connected @" << m_address << ":" << m_portTCP ;
	LOG_INFO(stream.str());
}

void Connection::Disconnect()
{
	receiveThread.detach();
	m_tcpSocket->disconnect();
	m_isConnected = false;


	std::stringstream stream;
	stream << "Client Disconnected @" << m_address << ":" << m_portTCP ;
	LOG_INFO(stream.str());
}

void Connection::ReceiveTCP(Queue<ServerMessage>& messageBuffer)
{
	const size_t maxMessageSize = 256;
	char buffer[maxMessageSize];
	size_t received;

	while (true)
	{
		std::memset(buffer, 0, maxMessageSize);

		auto data = m_tcpSocket->receive(buffer, maxMessageSize, received);
		if (data != sf::Socket::Done)
		{
			if (data == sf::Socket::Disconnected)
			{
				return;
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

		if(serverMessage.message.GetHeader().type == MessageType::CLIENT_SETUP)
		{
			m_isSetup = true;
			m_cv.notify_all();
			continue;
		}
		messageBuffer.enqueue(serverMessage);
	}
}

void Connection::SendTCP(const Message& msg) const
{
	m_network->GetThreadPool().enqueue([=]
	{
		auto buffer = msg.GetBuffer();
		if (m_tcpSocket->send(buffer.data(), buffer.size()) != sf::Socket::Done)
		{
			std::stringstream stream;
			stream << "Failed To send message over TCP to client: " << m_connectionID;
			LOG_ERROR(stream.str());
		}
	});;
}

void Connection::SendUDP(const Message& msg) const
{
	m_network->GetThreadPool().enqueue([=]
	{
		auto buffer = msg.GetBuffer();
		if (m_udpSocket->send(buffer.data(), buffer.size(), m_address, m_portUDP) != sf::Socket::Done)
		{
			std::stringstream stream;
			stream << "Failed To send message over UDP to client: " << m_connectionID;
			LOG_ERROR(stream.str());
		}
	});
}
