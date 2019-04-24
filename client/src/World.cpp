#include "World.h"
#include <iostream>
#include "shared/Random.h"
#include "Networking/ServerConnection.h"
#include "shared/Utility/Math.h"
#include "Graphics/SpriteManager.h"

World::World(const sf::RenderWindow& window) : m_window(window), m_camera(sf::Vector2f{ 0.0f,0.0f }, GetWindowSize(), 1024)
{
	m_wallSprite = SpriteManager::Instance().CreateSprite("Wall");
	m_floorSprite = SpriteManager::Instance().CreateSprite("Floor");
}


World::~World()
{
}

void World::Generate(ServerConnection* connection)
{
	m_serverConnection = connection;
	m_entityFactory.Setup();

	m_dungeon = std::unique_ptr<Dungeon>(new Dungeon(2, 2, 64, m_seed));
	m_dungeon->Generate();

	//Set dungeon colliders
	auto rects = m_dungeon->GetTileRectangles();
	for (auto& rect : rects)
	{
		auto collider = std::make_shared<Collider>(rect, CollisionLayer::WALL);
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
	if (m_entities.find(worldID) != m_entities.end()) //entity already exists in this world
		return nullptr;
	auto entity = m_entityFactory.CreateEntity(entityID, worldID, ownership, m_serverConnection, this);
	if (entity != nullptr)
	{
		entity->SetPosition(pos);
		entity->SetNetworkPosition(pos);
		entity->SetLastNetworkPosition(pos);
		entity->SetVelocity(velocity);
		entity->SetNetworkVelocity(velocity);
		m_entities.insert((std::make_pair(worldID, entity)));

		//add entity collider to the collider vector
		m_colliders.insert(entity->GetCollider());

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

const sf::RenderWindow& World::GetWindow() const
{
	return m_window;
}

void World::collisionDetection()
{
	std::unordered_set<std::shared_ptr<Collider>> toRemove;

	for (auto& entity : m_entities)
	{
		if (!entity.second->IsActive())
			continue;
		for (auto& other : m_colliders)
		{
			if (entity.second->GetCollider() == other)
				continue;

			if (other->CheckCollision(*entity.second->GetCollider()))
			{
				entity.second->OnCollision(*other);
			}
		}
		//set entity pos to collider pos
		entity.second->SetPosition(entity.second->GetCollider()->GetPosition());
	}
}

void World::removeEntity(unsigned int worldID)
{
	if (m_entities.find(worldID) != m_entities.end())
	{
		auto& entity = m_entities.at(worldID);
		//destroy collider
		const auto collider = std::find(m_colliders.begin(), m_colliders.end(), entity->GetCollider());
		if (collider != m_colliders.end())
		{
			m_colliders.erase(collider);
		}

		m_entities.erase(entity->GetWorldID());
	}
}

void World::ShootBullet(sf::Vector2f startPos, sf::Vector2f velocity)
{
	m_serverConnection->SendSpawnRequestMessage(1, startPos, velocity);
}

void World::RequestDestroyEntity(unsigned worldID)
{
	if (m_entities.find(worldID) != m_entities.end())
	{
		m_entities.at(worldID)->SetActive(false, false);
		m_serverConnection->SendEntityDestroyMessage(worldID);
	}
}


void World::Update(float deltaTime)
{
	if (!m_serverConnection->IsConnected() && !m_generated)
		return;

	for (auto& entity : m_entities)
	{
		if (entity.second->IsActive())
			entity.second->Update(deltaTime);
	}

	collisionDetection();
	//removeEntities();

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


	m_dungeon->Draw(window, m_wallSprite, m_floorSprite);

	for (auto& entity : m_entities)
	{
		if (entity.second->IsActive())
			entity.second->Draw(window);
	}

	//draw colliders
	if (m_debug) {
		for (auto& collider : m_colliders)
		{
			if (collider->IsActive())
				window.draw(collider->GetRect());
		}
	}
}
