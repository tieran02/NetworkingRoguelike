#include "ServerConnection.h"
#include <iostream>
#include <thread>
#include <cstring>
#include "shared/Message.h"
#include "shared/ConnectionMessage.h"
#include "shared/SpawnMessage.h"
#include "shared/MovementMessage.h"
#include "shared/Utility/Math.h"
#include "shared/Utility/Log.h"
#include <sstream>
#include "shared/EntityStateMessage.h"


ServerConnection::ServerConnection(unsigned short port, World* world) : m_world(world), m_broadcastUdpPort(port)
{
	updRecieve = std::thread(&ServerConnection::receiveUDP, this);
	tcpRecieve = std::thread(&ServerConnection::receiveTCP, this);
}


ServerConnection::~ServerConnection()
{

}

void ServerConnection::FindServer()
{
	while (!FoundServer())
	{
		LOG_INFO("Finding a server...");
		//send broadcast message
		const Message message(MessageType::BROADCAST, nullptr, 0, m_clientID);
		auto buffer = message.GetBuffer();
		if (m_serverUdpSocket.send(buffer.data(), buffer.size(), sf::IpAddress::Broadcast, m_broadcastUdpPort) != sf::Socket::Done)
		{
			LOG_FATAL("Failed To send broadcast message");
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
		LOG_FATAL("Failed to connect to sever TCP socket");
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
			LOG_TRACE("UDP::Received " + std::to_string(msg.message.GetHeader().size) + " bytes from server on port " + std::to_string(msg.senderPort));

			if (msg.message.GetHeader().type == MessageType::MOVEMENT)
			{
				MovementMessage* message = static_cast<MovementMessage*>(&msg.message);
				m_world->UpdateEntityPosition(message->WorldID(), message->GetPosition());
			}
			break;
		case Protocol::TCP:
			LOG_TRACE("TCP::Received " + std::to_string(msg.message.GetHeader().size) + " bytes from server on port " + std::to_string(msg.senderPort));

			if(msg.message.GetHeader().type == MessageType::CONNECTION_ID)
			{
				ConnectionMessage* message = static_cast<ConnectionMessage*>(&msg.message);
				LOG_INFO("Client ID = " + std::to_string(message->GetClientID()) + " World Seed = " + std::to_string(message->GetSeed()));

				m_world->SetSeed(message->GetSeed());
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
				LOG_INFO("Spawning Entity with ID = " + std::to_string(spawnMessage->GetEntityID()) + " and ownership of connection " + std::to_string(spawnMessage->GetOwnershipID())
					+ " @" + std::to_string(spawnMessage->GetPosition().x) + "," + std::to_string(spawnMessage->GetPosition().y));
				m_world->SpawnEntity(spawnMessage->GetEntityID(), spawnMessage->GetWorldID(), spawnMessage->GetPosition(), spawnMessage->GetOwnershipID());
			}
			else if (msg.message.GetHeader().type == MessageType::ENTITY_STATE)
			{
				EntityStateMessage* entityState = static_cast<EntityStateMessage*>(&msg.message);
				if(!entityState->IsActive())
				{
					m_world->GetEntities().erase(entityState->WorldID());
				}
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
	if (!m_isConnected)
		return;

	const Message disconnectMessage(MessageType::DISCONNECT, nullptr, 0, m_clientID);
	SendTcpMessage(disconnectMessage);
	m_isConnected = false;

	updRecieve.detach();
	tcpRecieve.detach();

	std::stringstream stream;
	stream << "Disconnected from Sever @" << m_serverAddress << ":" << m_serverTcpPort;
	LOG_INFO(stream.str());

}

void ServerConnection::SendUdpMessage(const Message& message)
{
    auto buffer = message.GetBuffer();
    if (m_serverUdpSocket.send(buffer.data(), buffer.size(), m_serverAddress, m_broadcastUdpPort) != sf::Socket::Done)
	{
		LOG_ERROR("Failed To send packet over UDP");
	}else
	{
		std::stringstream stream;
		stream << "Sent UDP message of size " << message.GetHeader().size << " to server";
		LOG_TRACE(stream.str());
	}
}

void ServerConnection::SendTcpMessage(const Message& message)
{
	auto buffer = message.GetBuffer();
	sf::Socket::Status status = m_serverTcpSocket.send(buffer.data(), buffer.size());
	if (status != sf::Socket::Done)
	{
		LOG_ERROR("Failed To send packet over TCP");
	}
	else
	{
		std::stringstream stream;
		stream << "Sent TCP message of size " << message.GetHeader().size << " to server";
		LOG_TRACE(stream.str());
	}
}

void ServerConnection::NotifyWorldGeneration()
{
	//send server a message over TCP confirming the client is setup
	Message setupMsg{ MessageType::CLIENT_SETUP, nullptr,0,(uint16_t)m_clientID };
	LOG_INFO("Notifying the server that the client has generated the level");
	SendTcpMessage(setupMsg);
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
		if (m_serverUdpSocket.getLocalPort() == 0)
			continue;

		sf::IpAddress sender;
		unsigned short port;
		size_t received;
		const size_t maxMessageSize = 256;
		char buffer[maxMessageSize];

		const auto receive = m_serverUdpSocket.receive(buffer, maxMessageSize, received, sender, port);
		if (receive != sf::Socket::Done)
		{
			LOG_ERROR("Failed To receive udp packet");
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
			LOG_ERROR("Failed To receive tcp packet");
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
