#include "ServerConnection.h"
#include <iostream>
#include <thread>
#include <cstring>
#include "shared/Message.h"
#include "shared/ConnectionMessage.h"
#include "shared/SpawnMessage.h"
#include "shared/MovementMessage.h"
#include "shared/Utility/Math.h"


ServerConnection::ServerConnection(unsigned short port, World* world) : m_world(world), m_broadcastUdpPort(port)
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
		//send broadcast message
		const Message message(MessageType::BROADCAST, nullptr, 0, m_clientID);
		auto buffer = message.GetBuffer();
		if (m_serverUdpSocket.send(buffer.data(), buffer.size(), sf::IpAddress::Broadcast, m_broadcastUdpPort) != sf::Socket::Done)
		{
			std::cerr << "Failed To send broadcast message\n";
		}

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
	sf::Socket::Status status = m_serverTcpSocket.connect(m_serverAddress, m_broadcastUdpPort+1);
	if (status != sf::Socket::Done)
	{
		std::cerr << "Failed to connect to sever TCP socket\n";
		return;
	}

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

			if (msg.message.GetHeader().type == MessageType::MOVEMENT)
			{
				MovementMessage* message = static_cast<MovementMessage*>(&msg.message);
				m_world->UpdateEntityPosition(message->WorldID(), message->GetPosition());
			}
			break;
		case Protocol::TCP:
			std::cout << "TCP::Received " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;

			if(msg.message.GetHeader().type == MessageType::CONNECTION_ID)
			{
				ConnectionMessage* message = static_cast<ConnectionMessage*>(&msg.message);
				std::cout << "Client ID = " << message->GetClientID() << " World Seed = " << message->GetSeed() << std::endl;
				m_world->SetSeed(message->GetSeed());
				m_serverUdpPort = message->GetUdpPort();
				m_serverTcpPort = m_serverTcpSocket.getRemotePort();
				m_serverIP = m_serverTcpSocket.getRemoteAddress();
				m_isConnected = true;
				m_clientID = message->GetClientID();
				//send client udp port to server 
				ConnectionMessage updPortMessage{ m_clientID ,0,m_serverUdpSocket.getLocalPort() };
				SendTcpMessage(updPortMessage);
			}
			else if(msg.message.GetHeader().type == MessageType::SPAWN)
			{
				SpawnMessage* spawnMessage = static_cast<SpawnMessage*>(&msg.message);
				std::cout << "Spawning Entity with ID = " << spawnMessage->GetEntityID() << " and ownership of connection " << spawnMessage->GetOwnershipID() << " @" << spawnMessage->GetPosition().x << "," << spawnMessage->GetPosition().y  << std::endl;
				m_world->SpawnEntity(spawnMessage->GetEntityID(), spawnMessage->GetWorldID(), spawnMessage->GetPosition(), spawnMessage->GetOwnershipID());
			}
			
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

void ServerConnection::SendUdpMessage(const Message& message)
{
    auto buffer = message.GetBuffer();
    if (m_serverUdpSocket.send(buffer.data(), buffer.size(), m_serverAddress, m_serverUdpPort) != sf::Socket::Done)
	{
		std::cerr << "Failed To send packet\n";
	}else
	{
		std::cout << "Sent UDP message of size " << message.GetHeader().size << " to server" << std::endl;

	}
}

void ServerConnection::SendTcpMessage(const Message& message)
{
    if (m_serverTcpSocket.getLocalPort() == 0)
		return;

	auto buffer = message.GetBuffer();
	if (m_serverTcpSocket.send(buffer.data(), buffer.size()) != sf::Socket::Done)
	{
		std::cerr << "Failed To send packet over TCP\n";
	}
	else
	{
		std::cout << "Sent TCP message of size " << message.GetHeader().size << " to server" << std::endl;
	}
}

void ServerConnection::NotifyWorldGeneration()
{
	m_waitTillGenerated.notify_all();
}

void ServerConnection::SendMovementMessage(unsigned int worldID, sf::Vector2f newPosition)
{
	//get entity
	if (m_world->GetEntities().find(worldID) != m_world->GetEntities().end())
	{
		auto& entity = m_world->GetEntities().at(worldID);

		//check if distance is greater than threshold
		const float distance = std::abs(Math::Distance(entity->GetNetworkPosition(), entity->GetPosition()));
		if (distance >= 16.0f) {
			MovementMessage message{ worldID,newPosition,sf::Vector2f{0.0f,0.0f} };
			SendUdpMessage(message);
		}
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

		//if the message is not a connection message we wait till the map has been generated
		if(serverMessage.message.GetHeader().type != MessageType::CONNECTION_ID)
		{
			std::unique_lock<std::mutex> lk(m_mutex);
			m_waitTillGenerated.wait(lk, [this] {return m_world->IsGenerated(); });
		}

		m_serverMessages.enqueue(serverMessage);
	}
}
