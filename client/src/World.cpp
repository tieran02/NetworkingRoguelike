#include "World.h"
#include <iostream>
#include "shared/Random.h"
#include "Networking/ServerConnection.h"

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

	m_dungeon = std::unique_ptr<Dungeon>(new Dungeon(2,2,m_seed));
	m_dungeon->Generate();
	m_generated = true;
	connection->NotifyWorldGeneration();
}

void World::SetSeed(unsigned int seed)
{
	m_seed = seed;
}

std::shared_ptr<Entity> World::SpawnEntity(unsigned int entityID, unsigned int worldID, sf::Vector2f pos, unsigned int ownership)
{
    auto entity = m_entityFactory.CreateEntity(entityID,worldID, ownership,m_serverConnection);
    if(entity != nullptr)
    {
		entity->SetPosition(pos);
		entity->SetNetworkPosition(pos);
		m_entities.insert((std::make_pair(worldID, entity)));
        return entity;
    }
    return nullptr;
}

void World::UpdateEntityPosition(unsigned worldID, sf::Vector2f newPosition)
{
	if(m_entities.find(worldID) != m_entities.end())
	{
		m_entities.at(worldID)->SetPosition(newPosition);
		m_entities.at(worldID)->SetNetworkPosition(newPosition);
	}
}


void World::Update()
{
	if (!m_serverConnection->IsConnected() && !m_generated)
		return;
	for (auto& entity : m_entities)
	{
		entity.second->Update();
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
