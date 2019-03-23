#include "Network.h"
#include "Connection.h"
#include <iostream>
#include <SFML/Network.hpp>
#include <thread>
#include <string.h>
#include "shared/Message.h"
#include "shared/ConnectionMessage.h"

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
	std::cout << "Enter level seed: ";
	std::cin >> m_seed;
	std::cout << std::endl;


	//BIND UPD
	if (m_udpSocket.bind(UDP_PORT) != sf::Socket::Done)
	{
		std::cerr << "failed to bind upd socket\n";
	}

	setupSocketSelector();
	std::thread updRecieve(&Network::receiveUDP, this);

	while (m_running)
	{
		while (!m_serverMessages.empty())
		{
			//Get GetData
            ServerMessage msg = m_serverMessages.dequeue();

			switch (msg.protocol)
			{
			case Protocol::UPD:
				if (msg.message.GetHeader().type == MessageType::BROADCAST) //UDP broadcast (used to find a server)
				{
					std::cout << "BROADCAST_MESSAGE from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;
					sendUdpMessage(MessageType::BROADCAST, nullptr, 0, msg.senderAddress, msg.senderPort);
				}
				else
					std::cout << "UDP::Received '" << msg.message.ToString() << "' " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;
				break;
			case Protocol::TCP:

				std::cout << "TCP::Received '" << msg.message.ToString() << "' " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;
				break;
			default:
				break;
			}
		}

	}
	m_running = false;

	updRecieve.detach();
}

void Network::sendUdpMessage(MessageType type, char* data, size_t size, sf::IpAddress address, unsigned short port)
{
	const Message message(type,data,size, 0);

	auto buffer = message.GetBuffer();
	if (m_udpSocket.send(buffer.data(), buffer.size(),address,port) != sf::Socket::Done)
	{
		std::cerr << "Failed To send packet\n";
	}
}

void Network::setupSocketSelector()
{
	if (listener.listen(TCP_PORT) != sf::Socket::Done)
	{
		std::cerr << "failed to listen on tcp\n";
	}

	selector.add(listener);

	//thread to wait for new connections
	std::thread([this]
	{
		while (true)
		{
			//Make the selecotr wait for any data on any socket
			if (selector.wait())
			{
				//check the listener
				if(selector.isReady(listener))
				{
					//add new connection (create a accept thread as the accept method is blocking)
					std::thread([this]
					{
						std::unique_ptr<sf::TcpSocket> client = std::unique_ptr<sf::TcpSocket>(new sf::TcpSocket());

						//accept new client
						if (listener.accept(*client) == sf::Socket::Done)
						{
							std::unique_lock<std::mutex> l(m_acceptMutex);

							//TODO: check if client with that id already exists if so reconnect with it
							std::unique_ptr<Connection> connection{ new Connection };
							const unsigned int id = m_connectionIdCount++;
							connection->Connect(client, id, m_seed);
							m_connections[id] = std::move(connection);
						}
					}).detach();
				}
				else
				{
					//the socket was not ready so test all the other connections
					for (auto& m_connection : m_connections)
					{
						sf::TcpSocket& client = m_connection.second->GetTcpSocket();
						if(selector.isReady(client))
						{
							m_connection.second->ReceiveTCP(m_serverMessages);
						}
					}
				}
			}
		}
	}).detach();
}

void Network::receiveUDP()
{
	while (m_running)
	{
		sf::IpAddress sender;
		unsigned short port;
		size_t received;
		const size_t maxMessageSize = 256;
		char buffer[maxMessageSize];

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
		m_serverMessages.enqueue(serverMessage);
	}
}
