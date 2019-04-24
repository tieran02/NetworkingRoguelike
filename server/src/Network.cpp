#include "Network.h"
#include "Connection.h"
#include <iostream>
#include <SFML/Network.hpp>
#include <thread>
#include <string.h>
#include "WorldState.h"
#include "shared/Utility/Log.h"
#include <sstream>


Network::Network(WorldState& world, unsigned short port) :m_worldState(&world), UDP_PORT(port), TCP_PORT(port+1)
{
}

Network::~Network()
{
}

void Network::Start()
{
	m_running = true;
	LOG_INFO("Running Server");

	//BIND UPD
	if (m_udpSocket.bind(UDP_PORT) != sf::Socket::Done)
	{
		LOG_FATAL("failed to bind upd socket");
	}

	m_threadPool.enqueue([this]
	{
		LOG_INFO("THREAD POOL TASK RAN 1");
	});

	std::thread acceptTCP(&Network::acceptTCP, this);
	std::thread updRecieve(&Network::receiveUDP, this);

	std::queue<std::tuple<unsigned int,Protocol,Message>> messagesToSend;

	float m_lastTickTime{0.0};
	float m_lastPing{ 0.0 };

	while (m_running)
	{
		m_currentTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		//Send message to clients
		if (m_currentTime >= m_lastTickTime + TICK_RATE)
		{
			if(m_currentTime >= m_lastPing + 1)
			{
				//ping all clients every second
				auto timestamp = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
				messagesToSend.push(std::make_tuple(0, Protocol::UPD, PingMessage(timestamp,0)));
				m_lastPing = m_currentTime;
				LOG_INFO("Server ms = " + std::to_string(m_currentTime - m_lastTickTime));
			}

			while (!messagesToSend.empty())
			{
				auto messageData = messagesToSend.front();
				messagesToSend.pop();

				unsigned int clientID = std::get<0>(messageData);
				Protocol protocol = std::get<1>(messageData);
				Message& message = std::get<2>(messageData);

				switch (protocol)
				{
				case Protocol::UPD:
					if (clientID == 0)
					{
						SendToAllUDP(message);
					}
					else
					{
						m_connections.at(clientID)->SendUDP(message);
					}
					break;
				case Protocol::TCP:
					if (clientID == 0)
					{
						SendToAllTCP(message);
					}
					else
					{
						m_connections.at(clientID)->SendTCP(message);
					}
					break;
				default:;
				}
			}

			m_lastTickTime = m_currentTime;
		}

		//poll messages
		while(!m_serverMessages.empty())
		{
			//Get GetData
            ServerMessage msg = m_serverMessages.dequeue();

			switch (msg.protocol)
			{
			case Protocol::UPD:
				if (msg.message.GetHeader().type == MessageType::BROADCAST) //UDP broadcast (used to find a server)
				{
					std::stringstream stream;
					stream << "BROADCAST_MESSAGE from " << msg.senderAddress << " on port " << msg.senderPort;
					LOG_INFO(stream.str());
					SendUdpMessage(BroadcastMessage(), msg.senderAddress, msg.senderPort);
				}
				else if (msg.message.GetHeader().type == MessageType::PING)
				{
					PingMessage* message = static_cast<PingMessage*>(&msg.message);
					calculateClientPing(message->GetHeader().id, message->GetTimeStamp());
				}
				else if (msg.message.GetHeader().type == MessageType::MOVEMENT)
				{
					MovementMessage* message = static_cast<MovementMessage*>(&msg.message);
					LOG_TRACE("Recieved Movement message from entity:" + std::to_string(message->WorldID()));
					//update world state with the new entity pos
					m_worldState->MoveEntity(message->WorldID(), message->GetPosition(), message->GetVelocity());
					//send movement message back to all clients (including itself)
					messagesToSend.push(std::make_tuple(0, Protocol::UPD, *message));
				}
				else if (msg.message.GetHeader().type == MessageType::ENTITY_STATE)
				{
					EntityStateMessage* message = static_cast<EntityStateMessage*>(&msg.message);
					LOG_TRACE("Recieved Entity state message from entity:" + std::to_string(message->WorldID()));
					//update world state with the new entity pos
					m_worldState->MoveEntity(message->WorldID(), message->GetPosition(), message->GetVelocity());
					//send entity state message back to all clients (including itself)
					messagesToSend.push(std::make_tuple(0, Protocol::UPD, *message));
				}
				else
				{
					std::stringstream stream;
					stream << "UDP::Received '" << msg.message.ToString() << "' " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort;
					LOG_TRACE(stream.str());
					//echo to all the other clients
					messagesToSend.push(std::make_tuple(0, Protocol::UPD, msg.message));
				}
				break;
			case Protocol::TCP:
				if (msg.message.GetHeader().type == MessageType::CONNECTION_ID) //set up client udp socket
				{
					ConnectionMessage* message = static_cast<ConnectionMessage*>(&msg.message);
					m_connections[message->GetClientID()]->m_portUDP = message->GetUdpPort();
					std::stringstream stream;
					stream << "Set Client " << message->GetClientID() << " UDP port to " << message->GetUdpPort();
					LOG_INFO(stream.str());

				}
				else if (msg.message.GetHeader().type == MessageType::DISCONNECT)
				{
					std::stringstream stream;
					stream << "Disconnecting Client " << msg.message.GetHeader().id;
					LOG_INFO(stream.str());

					//received disconnect msg from a client
					Disconnect(msg.message.GetHeader().id);
				}
				else if (msg.message.GetHeader().type == MessageType::ENTITY_STATE)
				{
					EntityStateMessage* message = static_cast<EntityStateMessage*>(&msg.message);

					if (!message->ShouldDestroy()) {
						LOG_TRACE("Recieved Entity state message from entity:" + std::to_string(message->WorldID()));
						//update world state with the new entity pos
 						m_worldState->MoveEntity(message->WorldID(), message->GetPosition(), message->GetVelocity());
						//send entity state message back to all clients (including itself)
						messagesToSend.push(std::make_tuple(0, Protocol::TCP, *message));
					}
					else
					{
						//delete entity
						LOG_TRACE("Recieved Entity delete message from entity:" + std::to_string(message->WorldID()));
						//TODO only delete if the client had ownership of the entity
						m_worldState->GetEntities().erase(message->WorldID());
						EntityStateMessage deleteMsg(message->WorldID(), sf::Vector2f{ 0,0 }, sf::Vector2f{ 0,0 }, false,true, 0);

						SendToAllTCP(deleteMsg);
					}
				}
				else if (msg.message.GetHeader().type == MessageType::SPAWN) //spawn request from client
				{
					SpawnMessage* requestMsg = static_cast<SpawnMessage*>(&msg.message);
					m_worldState->SpawnNewEntity(requestMsg->GetEntityID(), requestMsg->GetPosition(), requestMsg->GetVelocity(), requestMsg->GetOwnershipID());
				}
				else if(msg.message.GetHeader().type == MessageType::HEALTH)
				{
					HealthMessage* healthMessage = static_cast<HealthMessage*>(&msg.message);
					LOG_TRACE("Recieved Entity health message from entity:" + std::to_string(healthMessage->GetWorldID()));
					m_worldState->SetEntityHealth(healthMessage->GetWorldID(), healthMessage->GetHealth(), healthMessage->GetMaxHealth());
				}
				else
				{
					std::stringstream stream;
					stream << "TCP::Received '" << msg.message.ToString() << "' " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort;
					LOG_TRACE(stream.str());

					//echo to all the other clients
					messagesToSend.push(std::make_tuple(0, Protocol::TCP, msg.message));
				}
				break;
			default:
				break;
			}
		}

	}
	m_running = false;

	acceptTCP.join();
	updRecieve.join();
}

void Network::SendUdpMessage(const Message& message, sf::IpAddress address, unsigned short port)
{
	auto buffer = message.GetBuffer();
	if (m_udpSocket.send(buffer.data(), buffer.size(),address,port) != sf::Socket::Done)
	{
		LOG_ERROR("Failed To send packet");
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
			LOG_ERROR("Failed To receive udp packet");
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

void Network::acceptTCP()
{
	sf::TcpListener listener;

	if (listener.listen(TCP_PORT) != sf::Socket::Done)
	{
		LOG_FATAL("failed to listen on tcp");
	}

	while (m_running)
	{
		Connection* connection = new Connection(this);
		//accept new client
		if (listener.accept(*connection->GetTcpSocket()) == sf::Socket::Done)
		{
			Connect(connection);
		}
		else
		{
			delete connection;
		}
	}
}

void Network::sendWorldState()
{
	std::shared_lock<std::shared_mutex> lock{ m_worldState->GetEntityMutex() };
	//loop through all entities in the world
	auto& entities = m_worldState->GetEntities();
	MessageBatcher<EntityStateMessage> messageBatcher{ (int)entities.size() };

	int i = 0;
	for (auto& entity : entities)
	{
		EntityStateMessage msg{ entity.second->WorldID,entity.second->Position,sf::Vector2f{0,0},entity.second->IsActive,false,0 };
		messageBatcher.AddMessage(msg);
		i++;
	}

	LOG_INFO("Sending World State Update To All Clients");
	messageBatcher.SentToAllTCP(this);
}

void Network::calculateClientPing(unsigned id, long long clientTimestamp)
{
	const long long now = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
	const long long milliseconds = now - clientTimestamp;
	m_connections.at(id)->m_ping = milliseconds /2.0f;
}

void Network::SendToAllUDP(const Message& message, unsigned int ignore)
{
	m_threadPool.enqueue([=]
	{
		for (auto& connection : m_connections)
		{
			if (ignore != 0 && m_connections.find(ignore) != m_connections.end())
			{
				continue;
			}

			connection.second->SendUDP(message);
		}
	});
}

void Network::SendToAllTCP(const Message& message, unsigned int ignore)
{
	m_threadPool.enqueue([=] {
		for (auto& connection : m_connections)
		{
			if (ignore != 0 && m_connections.find(ignore) != m_connections.end())
			{
				continue;
			}

			connection.second->SendTCP(message);
		}
	});
}

void Network::SendSpawnMessage(unsigned int worldID, unsigned int entityID, sf::Vector2f position, sf::Vector2f velocity, unsigned int ownershipID)
{
	SpawnMessage message{ worldID, entityID,position,velocity,ownershipID };
	SendToAllTCP(message);
	std::stringstream stream;
	stream << "Spawning entity: worldID:" << worldID << " entityID:" << entityID << " ownershipID:" << ownershipID;
	LOG_INFO(stream.str());
}

void Network::SendMovementMessage(unsigned worldID, sf::Vector2f newPosition,sf::Vector2f velocity)
{
	MovementMessage message{ worldID,newPosition,velocity, 0};
	SendToAllUDP(message);
	LOG_TRACE("Sending movement message to all connections");

}

void Network::SendHealthMessage(unsigned worldID, float health, float maxHealth)
{
	HealthMessage message{ worldID,health,maxHealth, 0 };
	SendToAllTCP(message);
}

void Network::Connect(Connection* connection)
{
	std::thread([this, &connection]
	{
		std::unique_lock<std::mutex> l(m_connectionMutex);

		//TODO: check if client with that id already exists if so reconnect with it
		const unsigned int id = m_connectionIdCount++;
		m_connections[id] = std::unique_ptr<Connection>(connection);
		m_connections[id]->Connect(id, m_worldState->GetSeed(), m_serverMessages);
		l.unlock();

		//Send client connection data
		const ConnectionMessage message(id, m_worldState->GetSeed(), 0); //send the seed to the client (UDP port of the client will be sent back)
		m_connections[id]->SendTCP(message);

		//Spawn the player when the client is setup
		std::unique_lock<std::mutex> lock{ m_connections[id]->m_setupMutex };
		m_connections[id]->m_cv.wait(lock, [this, &id]() { return m_connections[id]->IsSetup(); });
		m_worldState->SpawnPlayer(*m_connections[id]);
	}).detach();
}

void Network::Disconnect(unsigned connectionID)
{
	std::unique_lock<std::mutex> l(m_connectionMutex);

	//disconect client
	m_connections[connectionID]->Disconnect();

	//remove connection from connection list
	m_connections.erase(connectionID);

	std::vector<std::shared_ptr<Entity>> entitiesToRemove;
	//remove all entities with the ownership associated with the connectionID
	for (auto& entity : m_worldState->GetEntities())
	{
		if(entity.second->OwnershipID == connectionID)
		{
			entitiesToRemove.push_back(entity.second);
		}
	}

	//sent entity state updates to other clients
	if(entitiesToRemove.size() > 1)
	{
		//batch messages
		BatchMessage<EntityStateMessage> batch(MessageType::BATCH,(int)entitiesToRemove.size());
		for (int i = 0; i < entitiesToRemove.size(); ++i)
		{
			EntityStateMessage entityState(entitiesToRemove[i]->WorldID, sf::Vector2f{ 0,0 }, sf::Vector2f{ 0,0 }, false,true, 0);
			batch[i] = entityState;
            m_worldState->GetEntities().erase(entitiesToRemove[i]->WorldID);
		}
		SendToAllTCP(batch);
	}
	else if(!entitiesToRemove.empty())
	{
		EntityStateMessage entityState(entitiesToRemove[0]->WorldID, sf::Vector2f{ 0,0 }, sf::Vector2f{ 0,0 }, false,true, 0);
		SendToAllTCP(entityState);
		m_worldState->GetEntities().erase(entitiesToRemove[0]->WorldID);
	}
}
