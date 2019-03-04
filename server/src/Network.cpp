#include "Network.h"
#include "Connection.h"
#include <iostream>
#include <SFML/Network.hpp>
#include <thread>
#include <string.h>
#include "shared/Message.h"

Network::Network(unsigned short port) : UDP_PORT(port), TCP_PORT(port+1)
{
}

Network::~Network()
{
}

void Network::Start()
{
	m_running = true;
	std::cout << "Running Server\n";

	//BIND UPD
	if (m_udpSocket.bind(UDP_PORT) != sf::Socket::Done)
	{
		std::cerr << "failed to bind upd socket\n";
	}

	std::thread tcpAcceptThread(&Network::acceptTCP,this);
	std::thread updRecieve(&Network::receiveUDP, this);
	std::thread tcpRecieve(&Network::receiveTCP, this);

	while (m_running)
	{
		while (!m_receivedMessages.empty())
		{
			//Get GetData
            ServerMessage msg = m_receivedMessages.dequeue();

			switch (msg.protocol)
			{
			case Protocol::UPD:
				if (msg.message.GetHeader().type == MessageType::BROADCAST) //UDP broadcast (used to find a server)
				{
					std::cout << "BROADCAST_MESSAGE from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;
					sendUdpMessage(MessageType::BROADCAST_RESPONSE, nullptr, 0, msg.senderAddress, msg.senderPort);
				}
				else
					std::cout << "UDP::Received '" << msg.message.ToString() << "' " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;
				break;
			case Protocol::TCP:
				if(msg.message.GetHeader().type == MessageType::CONNECTION_ID)
				{
					sendTcpMessage(MessageType::CONNECTION_ID, (char*)&msg.clientID, sizeof(unsigned int), msg.clientID);
				}

				std::cout << "TCP::Received '" << msg.message.ToString() << "' " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;
				break;
			default:
				break;
			}
		}

	}
	m_running = false;

	tcpAcceptThread.join();
	updRecieve.join();
	tcpRecieve.join();

}

void Network::sendUdpMessage(MessageType type, char* data, size_t size, sf::IpAddress address, unsigned short port)
{
	const Message message(type,data,size);

	auto buffer = message.GetBuffer();
	if (m_udpSocket.send(buffer.data(), buffer.size(),address,port) != sf::Socket::Done)
	{
		std::cerr << "Failed To send packet\n";
	}
}

void Network::sendUdpMessage(const std::string& string, sf::IpAddress address, unsigned short port)
{
	const Message message(MessageType::TEXT ,(char*)string.c_str(),string.size());

	auto buffer = message.GetBuffer();
	if (m_udpSocket.send(buffer.data(), buffer.size(), address, port) != sf::Socket::Done)
	{
		std::cerr << "Failed To send packet\n";
	}
}

void Network::sendTcpMessage(MessageType type, char* data, size_t size, int clientID)
{
	const Message message(type, data, size);

	auto buffer = message.GetBuffer();
	Connection* client = m_connections[clientID].get();
	if (client != nullptr)
	{
		if (client->GetTcpSocket().send(buffer.data(), buffer.size()) != sf::Socket::Done)
		{
			std::cerr << "Failed To send packet over TCP\n";
		}
	}
}

void Network::sendTcpMessage(const std::string& string, int clientID)
{
	const Message message(MessageType::TEXT, (char*)string.data(), string.size());

	auto buffer = message.GetBuffer();
	Connection* client = m_connections[clientID].get();
	if (client->GetTcpSocket().send(buffer.data(), buffer.size()) != sf::Socket::Done)
	{
		std::cerr << "Failed To send packet over TCP\n";
	}
}

void Network::acceptTCP()
{
	sf::TcpListener listener;
	if (listener.listen(TCP_PORT) != sf::Socket::Done)
	{
		std::cerr << "failed to listen on tcp\n";
	}

	while (m_running)
	{
		std::unique_ptr<sf::TcpSocket> client{ new sf::TcpSocket };

		if (listener.accept(*client) != sf::Socket::Done)

		{
			std::cerr << "failed to accept tcp client\n";
		}
		else
		{
			//TODO: check if client with that id already exists if so reconnect with it
			std::unique_ptr<Connection> connection{ new Connection };
			unsigned int id = m_connectionIdCount++;
			connection->Connect(client, id);
			m_connections[id] = std::move(connection);

			//send connection id to client
			//sendTcpMessage(MessageType::CONNECTION_ID, (char*)&id, sizeof(unsigned int), id);
		}
	};
}

void Network::receiveUDP()
{
	while (m_running)
	{
		sf::IpAddress sender;
		unsigned short port;
		size_t received;
		const size_t maxMessageSize = 256;
		char* buffer = new char[maxMessageSize];

		if (m_udpSocket.receive(buffer, maxMessageSize, received,sender,port) != sf::Socket::Done)
		{
			std::cerr << "Failed To receive udp packet\n";
			continue;
		}

		Message message{buffer};

		ServerMessage serverMessage(message);
		serverMessage.protocol = Protocol::UPD;
		serverMessage.senderAddress = sender;
		serverMessage.senderPort = port;
		m_receivedMessages.enqueue(serverMessage);

		delete[] buffer;

		//std::cout << std::endl;
		//std::cout << "received  " << serverMessage.message.ToString() << std::endl;
	}
}

void Network::receiveTCP()
{
	while (m_running)
	{
		for (auto& connection : m_connections)
		{
			if (connection.second == nullptr || !connection.second->IsConnected())
				continue;

			size_t received;
			const size_t maxMessageSize = 256;
			char* buffer = new char[maxMessageSize];

			auto send = connection.second->GetTcpSocket().receive(buffer, maxMessageSize,received);
			if (send != sf::Socket::Done)
			{
				if (send == sf::Socket::Disconnected)
				{
					connection.second->Disconnect();
					continue;;
				}
				std::cerr << "Failed To receive tcp packet\n";
				continue;
			}
			Message message{ buffer };

			ServerMessage serverMessage(message);
			serverMessage.protocol = Protocol::TCP;
			serverMessage.senderAddress = connection.second->GetAddress();
			serverMessage.senderPort = connection.second->GetPort();
			serverMessage.clientID = connection.first;
			m_receivedMessages.enqueue(serverMessage);
		}
	}
}
