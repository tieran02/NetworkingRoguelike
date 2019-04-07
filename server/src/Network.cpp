#include "Network.h"
#include "Connection.h"
#include <iostream>
#include <SFML/Network.hpp>
#include <thread>
#include <string.h>
#include "shared/Message.h"
#include "shared/ConnectionMessage.h"
#include "shared/BroadcastMessage.h"
#include "WorldState.h"
#include "shared/SpawnMessage.h"
#include "shared/MovementMessage.h"

Network::Network(WorldState& world, unsigned short port) :m_worldState(&world), UDP_PORT(port), TCP_PORT(port+1)
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

	//thread to wait for new connections
	std::thread socketLoop(&Network::socketLoop, this);

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
					SendUdpMessage(BroadcastMessage(), msg.senderAddress, msg.senderPort);
				}
				else
				{
					std::cout << "UDP::Received '" << msg.message.ToString() << "' " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;

					//echo to all the other clients
					SendToAllUDP(msg.message, msg.message.GetHeader().id);
				}
				break;
			case Protocol::TCP:
				if (msg.message.GetHeader().type == MessageType::CONNECTION_ID) //set up client udp socket
				{
					ConnectionMessage* message = static_cast<ConnectionMessage*>(&msg.message);
					m_connections[message->GetClientID()]->m_portUDP = message->GetUdpPort();
					std::cout << "Set Client " << message->GetClientID() << " UDP port to " << message->GetUdpPort() << std::endl;
				}
				else
				{
					std::cout << "TCP::Received '" << msg.message.ToString() << "' " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;
					//echo to all the other clients
					SendToAllTCP(msg.message, msg.message.GetHeader().id);
				}
				break;
			default:
				break;
			}
		}

	}
	m_running = false;

	socketLoop.detach();
	updRecieve.detach();
}

void Network::SendUdpMessage(const Message& message, sf::IpAddress address, unsigned short port)
{
	auto buffer = message.GetBuffer();
	if (m_udpSocket.send(buffer.data(), buffer.size(),address,port) != sf::Socket::Done)
	{
		std::cerr << "Failed To send packet\n";
	}
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

void Network::socketLoop()
{
	sf::TcpListener listener;
	sf::SocketSelector selector;

	if (listener.listen(TCP_PORT) != sf::Socket::Done)
	{
		std::cerr << "failed to listen on tcp\n";
	}

	selector.add(listener);
	while (true)
	{
		//Make the selecotr wait for any data on any socket
		if (selector.wait())
		{
			//check the listener
			if (selector.isReady(listener))
			{
				//add new connection (create a accept thread as the accept method is blocking)
				std::thread([&listener,&selector, this]
				{
					std::unique_ptr<Connection> connection = std::unique_ptr<Connection>(new Connection);
					//accept new client
					if (listener.accept(*connection->GetTcpSocket()) == sf::Socket::Done)
					{
						std::unique_lock<std::mutex> l(m_acceptMutex);

						//TODO: check if client with that id already exists if so reconnect with it
						const unsigned int id = m_connectionIdCount++;
						connection->Connect(id, m_worldState->GetSeed());
						m_connections[id] = std::move(connection);
						//Spawn the player
						m_worldState->SpawnPlayer(*m_connections[id]);
						selector.add(*m_connections[id]->GetTcpSocket());
						selector.add(*m_connections[id]->GetUdpSocket());
					}
				}).join();
			}
			else
			{
				//the socket was not ready so test all the other connections
				for (auto& connection : m_connections)
				{
					sf::TcpSocket& client = *connection.second->GetTcpSocket();
					if (selector.isReady(client))
					{
						std::thread([&connection, this] {
							connection.second->ReceiveTCP(m_serverMessages);
						}).join();
					}
					sf::UdpSocket& udpClient = *connection.second->GetUdpSocket();
					if (selector.isReady(udpClient))
					{
						std::thread([&connection, this] {
						connection.second->ReceiveUDP(m_serverMessages);
						}).join();
					}
				}
			}
		}
	}
}

void Network::SendToAllUDP(const Message& message, unsigned int ignore)
{
	for (auto& connection : m_connections)
	{
		if (ignore != 0 && m_connections.find(ignore) != m_connections.end())
		{
			continue;
		}

		connection.second->SendUDP(message);
	}
}

void Network::SendToAllTCP(const Message& message, unsigned int ignore)
{
	for (auto& connection : m_connections)
	{
		if (ignore != 0 && m_connections.find(ignore) != m_connections.end())
		{
			continue;
		}

		connection.second->SendTCP(message);
	}
}

void Network::SendSpawnMessage(unsigned int worldID, unsigned int entityID, sf::Vector2f position, unsigned int ownershipID)
{
	SpawnMessage message{ worldID, entityID,position,ownershipID };
	SendToAllTCP(message);
	std::cout << "Spawning entity: worldID:" << worldID << " entityID:" << entityID << " ownershipID:" << ownershipID;
}

void Network::SendMovementMessage(unsigned worldID, sf::Vector2f newPosition)
{
	MovementMessage message{ worldID,newPosition,sf::Vector2f{0,0} };
	SendToAllUDP(message);
	std::cout << "Sending movement message to all connections\n";

}
