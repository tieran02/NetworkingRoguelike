#include "ServerConnection.h"
#include <iostream>
#include <thread>
#include <cstring>
#include "shared/Utility/Math.h"
#include "shared/Utility/Log.h"
#include <sstream>


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

	m_tickClock.restart();
	m_lastTick = m_tickClock.getElapsedTime();
}

void ServerConnection::UpdateTick()
{
	sendEntityStates();
}

void ServerConnection::PollMessages()
{
	while (!m_serverMessages.empty())
	{
		//Get Data
		ServerMessage msg = m_serverMessages.dequeue();
		m_timeSinceLastMessage.restart();


		switch (msg.protocol)
		{
		case Protocol::UPD:
			LOG_TRACE("UDP::Received " + std::to_string(msg.message.GetHeader().size) + " bytes from server on port " + std::to_string(msg.senderPort));

			if (msg.message.GetHeader().type == MessageType::PING)
			{
				PingMessage* message = static_cast<PingMessage*>(&msg.message);
				calculatePing(message->GetTimeStamp());

			}
			else if (msg.message.GetHeader().type == MessageType::MOVEMENT)
			{
				MovementMessage* message = static_cast<MovementMessage*>(&msg.message);
				updateEntityNetworkState(message->WorldID(), message->GetPosition(),message->GetVelocity());
			}
			else if	(msg.message.GetHeader().type == MessageType::ENTITY_STATE)
			{
				EntityStateMessage* message = static_cast<EntityStateMessage*>(&msg.message);
				updateEntityNetworkState(message->WorldID(), message->GetPosition(), message->GetVelocity());

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
				auto pos = spawnMessage->GetPosition();
				m_world->SpawnEntity(spawnMessage->GetEntityID(), spawnMessage->GetWorldID(), spawnMessage->GetPosition(), spawnMessage->GetVelocity(), spawnMessage->GetOwnershipID());
			}
			else if (msg.message.GetHeader().type == MessageType::ENTITY_STATE)
			{
				// destroy entity
				EntityStateMessage* entityState = static_cast<EntityStateMessage*>(&msg.message);
				if(!entityState->IsActive())
				{
					m_world->removeEntity(entityState->WorldID());
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

void ServerConnection::sendEntityStates()
{
	//for each world entity update the entity state
	for (const auto& entity : m_world->GetEntities())
	{
		//onlu send movement of the entity if the client has ownership of it
		if (entity.second->hasOwnership()) {
			//check if the enity has exceeded its threshold
			const float distance = std::abs(Math::Distance(entity.second->GetNetworkPosition(), entity.second->GetPosition()));
			constexpr float threshold = 5.0f;
			if (distance >= threshold)
			{
				EntityStateMessage state{ entity.second->GetWorldID(),entity.second->GetPosition(),entity.second->GetVelocity(),true, m_clientID };
				SendUdpMessage(state);
				//Set network position and velocity to what we just sent to the server (This may get corrected later on when we receive a message)
				entity.second->SetNetworkPosition(entity.second->GetPosition());
				entity.second->SetNetworkVelocity(entity.second->GetVelocity());
			}
		}
	}
}

void ServerConnection::updateEntityNetworkState(unsigned worldID, sf::Vector2f newPosition, sf::Vector2f velocity)
{
	auto entities = m_world->GetEntities();
	if (entities.find(worldID) != entities.end())
	{
		entities.at(worldID)->SetLastNetworkPosition(entities.at(worldID)->GetNetworkPosition());
		entities.at(worldID)->SetNetworkPosition(newPosition);
		entities.at(worldID)->SetVelocity(velocity);
		entities.at(worldID)->SetNetworkVelocity(velocity);
	}
}

void ServerConnection::calculatePing(long long  serverTimestamp)
{
	const long long now = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
	const long long milliseconds = now - serverTimestamp;
	m_ping = milliseconds;;
	LOG_TRACE("Ping = " + std::to_string(m_ping));

	//send ping back to server
	SendUdpMessage(PingMessage{ serverTimestamp, m_clientID });
}

void ServerConnection::SendMovementMessage(unsigned int worldID, sf::Vector2f newPosition, sf::Vector2f velocity)
{
	//get entity
	if (m_world->GetEntities().find(worldID) != m_world->GetEntities().end())
	{
		auto& entity = m_world->GetEntities().at(worldID);

		//check if distance is greater than threshold
		const float distance = std::abs(Math::Distance(entity->GetNetworkPosition(), entity->GetPosition()));
		if (distance >= 16.0f || entity->GetVelocity() != entity->GetNetworkVelocity()) {
			const MovementMessage message{ worldID,newPosition,velocity, m_clientID };
			SendUdpMessage(message);
		}
	}
}

void ServerConnection::SendSpawnRequestMessage(unsigned entityID, sf::Vector2f position, sf::Vector2f velocity)
{
	const SpawnMessage message{ 0,entityID,position,velocity,m_clientID };
	SendTcpMessage(message);
}

void ServerConnection::SendEntityDestroyMessage(unsigned worldID)
{
	const EntityStateMessage state{ worldID,sf::Vector2f(),sf::Vector2f(),true, m_clientID };
	SendTcpMessage(state);
}

sf::Time ServerConnection::TimeSinceLastMessage() const
{
	return m_timeSinceLastMessage.getElapsedTime();
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
