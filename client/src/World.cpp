#include "World.h"
#include <iostream>
#include "shared/Random.h"
#include "Networking/ServerConnection.h"
#include "shared/Utility/Math.h"

World::World()
{
}


World::~World()
{
}

void World::Generate(ServerConnection* connection)
{
	m_serverConnection = connection;
    m_entityFactory.Setup();

	m_dungeon = std::unique_ptr<Dungeon>(new Dungeon(2,2,32,m_seed));
	m_dungeon->Generate();
	m_generated = true;
	connection->NotifyWorldGeneration();
}

void World::SetSeed(unsigned int seed)
{
	m_seed = seed;
}

std::shared_ptr<Entity> World::SpawnEntity(unsigned int entityID, unsigned int worldID, sf::Vector2f pos, sf::Vector2f velocity, unsigned int ownership)
{
    auto entity = m_entityFactory.CreateEntity(entityID,worldID, ownership,m_serverConnection, this);
    if(entity != nullptr)
    {
		entity->SetPosition(pos);
		entity->SetNetworkPosition(pos);
		entity->SetNetworkVelocity(velocity);
		m_entities.insert((std::make_pair(worldID, entity)));
        return entity;
    }
    return nullptr;
}

//void World::UpdateEntityPosition(unsigned worldID, sf::Vector2f newPosition, sf::Vector2f velocity)
//{
//	if(m_entities.find(worldID) != m_entities.end())
//	{
//		m_entities.at(worldID)->SetPosition(newPosition);
//		m_entities.at(worldID)->SetNetworkPosition(newPosition);
//		m_entities.at(worldID)->SetVelocity(velocity);
//		m_entities.at(worldID)->SetNetworkVelocity(velocity);
//	}
//}

void World::SetWindowFocused(bool focused)
{
	m_windowFocused = focused;
}

bool World::IsWindowFocused() const
{
	return m_windowFocused;
}


void World::Update(float deltaTime)
{
	if (!m_serverConnection->IsConnected() && !m_generated)
		return;

	for (auto& entity : m_entities)
	{
		entity.second->Update(deltaTime);
	}
}


void World::Draw(sf::RenderWindow & window)
{
	if (!m_serverConnection->IsConnected() && !m_generated)
		return;

	m_dungeon->Draw(window);

	for(auto& entity : m_entities)
	{
		entity.second->Draw(window);
	}
}
