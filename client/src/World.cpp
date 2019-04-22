#include "World.h"
#include <iostream>
#include "shared/Random.h"
#include "Networking/ServerConnection.h"
#include "shared/Utility/Math.h"

World::World() : m_camera( sf::Vector2f{ 0.0f,0.0f }, GetWindowSize(),1024 )
{
}


World::~World()
{
}

void World::Generate(ServerConnection* connection)
{
	m_serverConnection = connection;
    m_entityFactory.Setup();

	m_dungeon = std::unique_ptr<Dungeon>(new Dungeon(2,2,64,m_seed));
	m_dungeon->Generate();

	//Set dungeon colliders
	auto rects = m_dungeon->GetTileRectangles();
	for (auto& rect : rects)
	{
		auto collider = std::make_shared<Collider>(rect);
		m_colliders.insert(collider);
	}

	m_generated = true;
	connection->NotifyWorldGeneration();

	m_camera.SetPosition(sf::Vector2f{ 2048.0f ,2048.0f });
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
		entity->SetLastNetworkPosition(pos);
		entity->SetNetworkVelocity(velocity);
		//add entity collider to the collider vector
		m_colliders.insert(entity->GetCollider());

		m_entities.insert((std::make_pair(worldID, entity)));

		entity->Start();
        return entity;
    }
    return nullptr;
}

void World::SetWindowFocused(bool focused)
{
	m_windowFocused = focused;
}

bool World::IsWindowFocused() const
{
	return m_windowFocused;
}

void World::SetWindowSize(sf::Vector2u size)
{
	m_camera.SetSize(size, m_camera.GetSize());
	m_windowSize = size;
}

sf::Vector2u World::GetWindowSize() const
{
	return m_windowSize;
}

Camera& World::GetCamera()
{
	return m_camera;
}

void World::collisionDetection()
{
	std::unordered_set<std::shared_ptr<Collider>> toRemove;

	for (auto& entity : m_entities)
	{
		for (auto& other : m_colliders)
		{
			if(entity.second->GetCollider().get() == other.get())
				continue;

			other->CheckCollision(*entity.second->GetCollider());
		}
		//set entity pos to collider pos
		entity.second->SetPosition(entity.second->GetCollider()->GetPosition());
	}
}


void World::Update(float deltaTime)
{
	if (!m_serverConnection->IsConnected() && !m_generated)
		return;

	for (auto& entity : m_entities)
	{
		entity.second->Update(deltaTime);
	}

	collisionDetection();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F4))
	{
		m_debug = !m_debug;
	}
}


void World::Draw(sf::RenderWindow& window)
{
	if (!m_serverConnection->IsConnected() && !m_generated)
		return;

	window.setView(m_camera.GetView());


	m_dungeon->Draw(window);

	for(auto& entity : m_entities)
	{
		entity.second->Draw(window);
	}

	//draw colliders
	if (m_debug) {
		for (auto& collider : m_colliders)
		{
			window.draw(collider->GetRect());
		}
	}
}
