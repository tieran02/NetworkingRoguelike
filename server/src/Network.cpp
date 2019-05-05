#include "Network.h"
#include "Connection.h"
#include <iostream>
#include <SFML/Network.hpp>
#include <thread>
#include <string.h>
#include "WorldState.h"
#include "shared/Utility/Log.h"
#include <sstream>
#include "shared/Collider.h"
#include "shared/EntityDataManager.h"
#include "Connection.h"

Network::Network(WorldState& world, unsigned short port) : m_worldState(&world), UDP_PORT(port), TCP_PORT(port + 1), m_lobby(this)
{
}

Network::~Network()
{
    m_close = true;
    m_receiveTcpThread.detach();
    m_receiveUdpThread.detach();
    LOG_INFO("Calling network destructor!");
}

void Network::Start()
{
	m_running = true;
	LOG_INFO("Running Server");

	//BIND UPD
	if (m_udpSocket.bind(UDP_PORT) != sf::Socket::Done)
	{
		LOG_FATAL("failed to bind upd socket");
		return;
	}

    m_receiveUdpThread = std::thread(&Network::receiveUDP, this);;
    m_receiveTcpThread = std::thread(&Network::acceptTCP, this);;

	//Start Lobby
	m_lobby.Create(4);

	float m_lastTickTime{ 0.0 };
	float m_lastPing{ 0.0 };

	while (m_running)
	{
		//wait for lobby to start the game
		if (!m_lobby.ShouldStart())
		{
			pollMessages();
			m_lobby.Update();
			continue;
		}

		//are player still in the game? if not return to lobby
		if (m_connections.empty())
		{
			LOG_INFO("Remaking lobby");
			m_worldState->GenerateWorld();
			m_lobby.Create(4);
		}



		m_currentTime = std::chrono::duration<float>(std::chrono::steady_clock::now().time_since_epoch()).count();


		//Send message to clients
		if (m_currentTime >= m_lastTickTime + TICK_RATE)
		{
			pollMessages();
			m_worldState->Update();


			if (m_currentTime >= m_lastPing + 1)
			{
				//ping all clients every second
				const long long now = std::chrono::steady_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
				//SendToAllUDP(PingMessage(now, 0));
				m_lastPing = m_currentTime;
				//LOG_INFO("Server ms = " + std::to_string(m_currentTickRate));
			}

			while (!m_messagesToSend.empty())
			{
				auto messageData = m_messagesToSend.front();
				m_messagesToSend.pop();

				unsigned int clientID = std::get<0>(messageData);
				Protocol protocol = std::get<1>(messageData);
				Message& message = std::get<2>(messageData);
				unsigned int ignore = std::get<3>(messageData);

				switch (protocol)
				{
				case Protocol::UPD:
					if (clientID == 0)
					{
						SendToAllUDP(message,ignore);
					}
					else
					{
						m_connections.at(clientID)->SendUDP(message);
					}
					break;
				case Protocol::TCP:
					if (clientID == 0)
					{
						SendToAllTCP(message, ignore);
					}
					else
					{
						m_connections.at(clientID)->SendTCP(message);
					}
					break;
				default:;
				}
			}

			//sent update data to clients
			sendWorldState();

			m_currentTickRate = m_currentTime - m_lastTickTime;
			m_lastTickTime = m_currentTime;
		}
	}
	m_running = false;
}

void Network::SendUdpMessage(const Message& message, sf::IpAddress address, unsigned short port)
{
	auto buffer = message.GetBuffer();
	if (m_udpSocket.send(buffer.data(), buffer.size(), address, port) != sf::Socket::Done)
	{
		LOG_ERROR("Failed To send packet");
	}
}

void Network::pollMessages()
{
	//poll messages
	while (!m_serverMessages.empty())
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

				//update world state with the new entity velocity
				m_worldState->updateEntityVelocityFromClient(message->WorldID(), message->GetVelocity());
			}
			else
			{
				std::stringstream stream;
				stream << "UDP::Received '" << msg.message.ToString() << "' " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort;
				LOG_TRACE(stream.str());
				//echo to all the other clients
				m_messagesToSend.push(std::make_tuple(0, Protocol::UPD, msg.message, 0));
			}
			break;
		case Protocol::TCP:
			if (msg.message.GetHeader().type == MessageType::CONNECTION_ID) //set up client udp socket
			{
				ConnectionMessage* message = static_cast<ConnectionMessage*>(&msg.message);
				m_connections[message->GetClientID()]->m_portUDP = message->GetUdpPort();
				m_connections[message->GetClientID()]->SetClientName(message->GetName());

				std::string names = "";
				//Sent client names back to all clients
				for (const auto& connection : m_connections)
				{
					names.append( "," + std::to_string(connection.first) + ":" + connection.second->GetName());
				}
				names.erase(0, 1); //remove first comma
				TextMessage clientNames{ names,TextType::PLAYER_NAMES, 0};
				SendToAllTCP(clientNames);

				std::stringstream stream;
				stream << "Set Client: " << message->GetName() << "::" << message->GetClientID() << " UDP port to " << message->GetUdpPort();
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
					m_messagesToSend.push(std::make_tuple(0, Protocol::TCP, *message, 0));
					//set active or not
					m_worldState->SetEntityActive(message->WorldID(), message->IsActive());
				}
				else
				{
					//delete entity
					LOG_TRACE("Recieved Entity delete message from entity:" + std::to_string(message->WorldID()));
					//TODO only delete if the client had ownership of the entity
					m_worldState->DestroyEntity(message->WorldID());
					EntityStateMessage deleteMsg(message->WorldID(), sf::Vector2f{ 0,0 }, sf::Vector2f{ 0,0 }, false, true, 0);

					SendToAllTCP(deleteMsg);
				}
			}
			else if (msg.message.GetHeader().type == MessageType::SPAWN) //spawn request from client
			{
				SpawnMessage* requestMsg = static_cast<SpawnMessage*>(&msg.message);

				//TODO get name from Entity data manager
				const auto& entityName = EntityDataManager::Instance().GetEntityData(requestMsg->GetEntityID()).EntityName;
				auto entity = m_worldState->SpawnNewEntity(entityName, requestMsg->GetPosition(), requestMsg->GetVelocity(), requestMsg->GetOwnershipID(), requestMsg->GetLayerOverride());
				entity->SetClientVelocity(requestMsg->GetVelocity());
			}
			else if (msg.message.GetHeader().type == MessageType::HEALTH)
			{
				HealthMessage* healthMessage = static_cast<HealthMessage*>(&msg.message);
				LOG_TRACE("Recieved Entity health message from entity:" + std::to_string(healthMessage->GetWorldID()));
				m_worldState->SetEntityHealth(healthMessage->GetWorldID(), healthMessage->GetHealth(), healthMessage->GetMaxHealth());
			}
			else if (msg.message.GetHeader().type == MessageType::GAME_START)
			{
				if (!m_lobby.ShouldStart())
				{
					SendToAllTCP(msg.message);
					m_lobby.Start();
				}
			}
			else if (msg.message.GetHeader().type == MessageType::TEXT)
			{
				TextMessage* message = static_cast<TextMessage*>(&msg.message);

				//check if the text is a chat message
				if (message->GetTextType() == TextType::CHAT)
				{
					SendToAllTCP(msg.message, message->GetHeader().id); //send to all other clients
				}
			}
			else
			{
				std::stringstream stream;
				stream << "TCP::Received '" << msg.message.ToString() << "' " << msg.message.GetHeader().size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort;
				LOG_TRACE(stream.str());

				//echo to all the other clients
				m_messagesToSend.push(std::make_tuple(0, Protocol::TCP, msg.message, 0));
			}
			break;
		default:
			break;
		}
	}
}

void Network::receiveUDP()
{
	while (!m_close)
	{
		sf::IpAddress sender;
		unsigned short port;
		size_t received;
		const size_t maxMessageSize = 256;
		char buffer[maxMessageSize];

		if (m_udpSocket.receive(buffer, maxMessageSize, received, sender, port) != sf::Socket::Done)
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

void Network::acceptTCP()
{
	sf::TcpListener listener;

	if (listener.listen(TCP_PORT) != sf::Socket::Done)
	{
		LOG_FATAL("failed to listen on tcp");
		return;
	}

	while (!m_close)
	{
		//accept new client
        std::unique_ptr<sf::TcpSocket> socket(new sf::TcpSocket);
		if (listener.accept(*socket) == sf::Socket::Done)
		{
			if (m_lobby.IsFull())
			{
				LOG_WARN("Client tried connecting but the game is full");
				TextMessage msg("Failed to join game (game is full)", TextType::PLAIN,0);
				auto buffer = msg.GetBuffer();
				socket->send(buffer.data(), buffer.size());
				socket->disconnect();
			}
			else if (m_lobby.ShouldStart())
			{

				LOG_WARN("Client tried connecting but the game is in-progress");
				TextMessage msg("Failed to join game (game in progress)", TextType::PLAIN,0);
				auto buffer = msg.GetBuffer();
				socket->send(buffer.data(), buffer.size());
				socket->disconnect();
			}
			else 
			{
				Connect(std::move(socket));
			}
		}
	}
}

void Network::sendWorldState()
{
	std::unique_lock<std::mutex> lock{ m_worldState->GetEntityMutex() };
	for (const auto& entity : m_worldState->GetEntities())
	{
		if (entity.second->Velocity() != entity.second->ClientVelocity())
		{
			entity.second->SetClientVelocity(entity.second->Velocity()); //set client velocity to current velocity
			//sent new velocity to client
			SendMovementMessage(entity.first, entity.second->Velocity());
			LOG_INFO("sending updated velocity from server to client");
		}
	}

	if (ticksSinceReSync++ >= 32 * 1) 
	{
		for (const auto& entity : m_worldState->GetEntities())
		{
			//resync all entities
			EntityStateMessage msg{ entity.second->WorldID(),entity.second->Position(),entity.second->Velocity(),entity.second->IsActive(), false, 0 };
			SendToAllTCP(msg);
		}
		ticksSinceReSync = 0;
	}
}

void Network::calculateClientPing(unsigned id, long long clientTimestamp)
{
	const long long now = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
	const long long milliseconds = now - clientTimestamp;
	m_connections.at(id)->m_ping = milliseconds / 2.0f;
}

void Network::SendToAllUDP(const Message& message, unsigned int ignore)
{
	if (m_connections.empty())
		return;

	for (auto& connection : m_connections)
	{
		if (ignore != 0 && connection.first == ignore)
		{
			continue;
		}

		connection.second->SendUDP(message);
	}
}

void Network::SendToAllTCP(const Message& message, unsigned int ignore)
{
	if (m_connections.empty())
		return;

	for (auto& connection : m_connections)
	{
		if (ignore != 0 && connection.first == ignore)
		{
			continue;
		}

		connection.second->SendTCP(message);
	}
}

void Network::SendSpawnMessage(unsigned int worldID, unsigned int entityID, sf::Vector2f position, sf::Vector2f velocity, unsigned int ownershipID, CollisionLayer layerOverride)
{
	const SpawnMessage message{ worldID, entityID,position,velocity,ownershipID,layerOverride };
	SendToAllTCP(message);
	std::stringstream stream;
	stream << "Spawning entity: worldID:" << worldID << " entityID:" << entityID << " ownershipID:" << ownershipID;
	LOG_INFO(stream.str());
}

void Network::SendMovementMessage(unsigned worldID, sf::Vector2f velocity)
{
	MovementMessage message{ worldID,velocity, 0 };
	SendToAllUDP(message);
	LOG_TRACE("Sending movement message to all connections");

}

void Network::SendHealthMessage(unsigned worldID, float health, float maxHealth)
{
	HealthMessage message{ worldID,health,maxHealth, 0 };
	SendToAllTCP(message);
}

void Network::Connect(std::unique_ptr<sf::TcpSocket> socket)
{
	//check if the game is full
	if(m_lobby.IsFull())
	{
		LOG_WARN("Game is full! can't connect new player");
		//send message to new socket
		std::string msg{ "Game IS FULL!" };
		socket->send(msg.data(), msg.size());
		return;
	}

	unsigned int id {0};
    {
        std::unique_lock<std::mutex> l(m_connectionMutex);

        //TODO: check if client with that id already exists if so reconnect with it
        id = m_connectionIdCount++;
        m_connections.insert(std::make_pair(id,std::unique_ptr<Connection>(new Connection(this))));
        m_connections.at(id)->m_tcpSocket = std::move(socket);
        m_connections.at(id)->Connect(id, m_worldState->GetSeed());

        //Send client connection data
        const ConnectionMessage message(id,"ServerPlayerName", m_worldState->GetSeed(), 0); //send the seed to the client (UDP port of the client will be sent back)
        m_connections.at(id)->SendTCP(message);

		m_lobby.AddPlayerToLobby();
    }

	//check if game is in progress or join lobby
	if (!m_lobby.ShouldStart())
	{
		//lobby
		LOG_INFO("Client: " + std::to_string(id) + " entered the lobby");
	}
	else  //Spawn the player when the client is setup and not in lobby
	{
		//TODO IGNORE CONNECTIONS
    }
}

void Network::Disconnect(unsigned int connectionID)
{
	std::unique_lock<std::mutex> l(m_connectionMutex);

	//disconect client
	m_connections[connectionID]->Disconnect();

	m_lobby.PlayerDisconnected();

	//remove connection from connection list
	m_connections.erase(connectionID);

	std::vector<std::shared_ptr<Entity>> entitiesToRemove;
	//remove all entities with the ownership associated with the connectionID
	{
		std::unique_lock<std::mutex> lock{ m_worldState->GetEntityMutex() };

		for (auto& entity : m_worldState->GetEntities())
		{
			if (entity.second->OwnershipID() == connectionID)
			{
				entitiesToRemove.push_back(entity.second);
			}
		}


		for (auto& entity : entitiesToRemove)
		{
			EntityStateMessage entityState(entity->WorldID(), sf::Vector2f{ 0,0 }, sf::Vector2f{ 0,0 }, false, true, 0);
			SendToAllTCP(entityState);
			m_worldState->GetEntities().erase(entity->WorldID());
		}
	}

	std::string names = "";
	//Sent client names back to all clients
	for (const auto& connection : m_connections)
	{
		names.append("," + std::to_string(connection.first) + ":" + connection.second->GetName());
	}
	names.erase(0, 1); //remove first comma
	TextMessage clientNames{ names,TextType::PLAYER_NAMES, 0 };
	SendToAllTCP(clientNames);
}

void Network::SpawnPlayer(unsigned int connectionID)
{
	//make sure the player is ready then spawn the player 
	std::unique_lock<std::mutex> lock{ m_connections[connectionID]->m_setupMutex };
	m_connections.at(connectionID)->m_cv.wait(lock, [this, &connectionID]() { return m_connections[connectionID]->IsSetup(); });
	m_worldState->SpawnPlayer(*m_connections.at(connectionID));
}
