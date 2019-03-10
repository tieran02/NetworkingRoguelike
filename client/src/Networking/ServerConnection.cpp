#include "ServerConnection.h"
#include <iostream>
#include <thread>
#include <cstring>
#include "shared/Message.h"
#include "shared/ConnectionMessage.h"


ServerConnection::ServerConnection(unsigned short port) : m_serverUdpPort(port), m_serverTcpPort(port+1)
{
	updRecieve = std::thread(&ServerConnection::receiveUDP, this);
	tcpRecieve = std::thread(&ServerConnection::receiveTCP, this);
}


ServerConnection::~ServerConnection()
{
	updRecieve.detach();
	tcpRecieve.detach();
}

void ServerConnection::FindServer()
{
	while (!FoundServer())
	{
		std::cout << "Finding a server...\n";
		sendUdpMessage(MessageType::BROADCAST, nullptr, 0, sf::IpAddress::Broadcast, m_serverUdpPort);
		sf::sleep(sf::milliseconds(100));

		//poll messages
		PollMessages();
		sf::sleep(sf::milliseconds(900));
	}

}

void ServerConnection::Connect()
{
	if(!FoundServer() && !m_isConnected)
		return;

	//TCP SOCKET
	sf::Socket::Status status = m_serverTcpSocket.connect(m_serverAddress, m_serverTcpPort);
	if (status != sf::Socket::Done)
	{
		std::cerr << "Failed to connect to sever TCP socket\n";
		return;
	}

	sendTcpMessage(MessageType::CONNECTION_ID, nullptr, 0);

	//request for client ID
	while (!m_isConnected)
	{
		PollMessages();
	}
}

void ServerConnection::PollMessages()
{
	while (!m_serverMessages.empty())
	{
		//Get Data
		ServerMessage msg = m_serverMessages.dequeue();

		switch (msg.protocol)
		{
		case Protocol::UPD:
			std::cout << "UDP::Received " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;
			break;
		case Protocol::TCP:
			std::cout << "TCP::Received " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;

			if(msg.message.GetHeader().type == MessageType::CONNECTION_ID)
			{
                ConnectionMessage* message = static_cast<ConnectionMessage*>(&msg.message);
				std::cout << "Client ID = " << message->GetClientID() <<" World Seed = " << message->GetSeed() << std::endl;
				m_seed = message->GetSeed();
				m_isConnected = true;
			}
			break;
		default:
			break;
		}


		if (!FoundServer() && msg.message.GetHeader().type == MessageType::BROADCAST) //server response from broadcast sent
		{
			m_serverAddress = msg.senderAddress;
		}
	}
}

bool ServerConnection::FoundServer() const
{
	return m_serverAddress != sf::IpAddress::None;
}

bool ServerConnection::IsConnected() const
{
	return m_isConnected;
}

void ServerConnection::Disconnect()
{
	m_serverTcpSocket.disconnect();
	m_isConnected = false;
	std::cout << "Disconnected from Sever @" << m_serverAddress << ":" << m_serverTcpPort << std::endl;

}

void ServerConnection::sendUdpMessage(MessageType type, char* data, size_t size, sf::IpAddress address, unsigned short port)
{
	const Message message(type, data, size);

	auto buffer = message.GetBuffer();
	if (m_serverUdpSocket.send(buffer.data(), buffer.size(), address, port) != sf::Socket::Done)
	{
		std::cerr << "Failed To send packet\n";
	}
}

void ServerConnection::sendUdpMessage(const std::string& string, sf::IpAddress address, unsigned short port)
{
	const Message message(MessageType::TEXT, (char*)string.c_str(), string.size());

	auto buffer = message.GetBuffer();
	if (m_serverUdpSocket.send(buffer.data(), buffer.size(), address, port) != sf::Socket::Done)
	{
		std::cerr << "Failed To send packet\n";
	}
	std::cout << "Sending   " << message.GetData() << std::endl;
}

void ServerConnection::sendTcpMessage(MessageType type, char* data, size_t size)
{
	if (m_serverTcpSocket.getLocalPort() == 0)
		return;

	const Message message(type, data, size);

	auto buffer = message.GetBuffer();
	if (m_serverTcpSocket.send(buffer.data(), buffer.size()) != sf::Socket::Done)
	{
		std::cerr << "Failed To send packet over TCP\n";
	}
}

void ServerConnection::sendTcpMessage(const std::string& string)
{
	if (m_serverTcpSocket.getLocalPort() == 0)
		return;

	const Message message(MessageType::TEXT, (char*)string.data(), string.size());

	auto buffer = message.GetBuffer();
	if (m_serverTcpSocket.send(buffer.data(), buffer.size()) != sf::Socket::Done)
	{
		std::cerr << "Failed To send packet over TCP\n";
	}
}

void ServerConnection::receiveUDP()
{
	while (true)
	{
		sf::IpAddress sender;
		unsigned short port;
		size_t received;
		const size_t maxMessageSize = 256;
		char buffer[maxMessageSize];

		const auto receive = m_serverUdpSocket.receive(buffer, maxMessageSize, received, sender, port);
		if (receive != sf::Socket::Done)
		{
			std::cerr << "Failed To receive udp packet\n";
			continue;
		}

		Message message{ buffer };
		ServerMessage serverMessage(message);
		serverMessage.protocol = Protocol::UPD;
		serverMessage.senderAddress = sender;
		serverMessage.senderPort = port;
		m_serverMessages.enqueue(serverMessage);

	}
}

void ServerConnection::receiveTCP()
{
	while (true)
	{
		if (m_serverTcpSocket.getLocalPort() == 0)
			continue;

		size_t received;
		const size_t maxMessageSize = 256;
		char buffer[maxMessageSize];

		const auto receive = m_serverTcpSocket.receive(buffer, maxMessageSize, received);
		if (receive != sf::Socket::Done)
		{
			if (receive == sf::Socket::Disconnected)
			{
				Disconnect();
				continue;
			}
			std::cerr << "Failed To receive tcp packet\n";
			continue;

		}
		Message message{ buffer };

		ServerMessage serverMessage(message);
		serverMessage.protocol = Protocol::TCP;
		serverMessage.senderAddress = m_serverTcpSocket.getRemoteAddress();
		serverMessage.senderPort = m_serverTcpSocket.getRemotePort();
		m_serverMessages.enqueue(serverMessage);
	}
}
