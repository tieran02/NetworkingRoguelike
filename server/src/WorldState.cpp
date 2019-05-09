#include "WorldState.h"
#include "Network.h"
#include "shared/Random.h"
#include <memory>
#include "shared/Utility/Log.h"


WorldState::WorldState(unsigned int seed) : m_seed(seed)
{
    //ctor
}

WorldState::~WorldState()
{
    //dtor
}

void WorldState::GenerateWorld()
{
	entityIdCounter = 0;
	m_entities.clear();
	m_enemies.clear();
	m_players.clear();

	m_dungeon = std::unique_ptr<Dungeon>(new Dungeon(2, 2,64, m_seed));
	m_dungeon->Generate();

	SpawnEnemies();
	SpawnPickups();
}

void WorldState::Update()
{
	//loop through all enemies
	for (const auto& enemy : m_enemies)
	{
		if(!enemy.second->IsActive())
			continue;

		if(enemy.second->GetTarget() == nullptr || !enemy.second->GetTarget()->IsActive())
		{
			//look for closest player
			float distance = 0.0f;
			const auto player = GetClosestPlayer(*enemy.second, distance);
			if (player != nullptr)
			{
				if (distance <= 320.0f)
				{
					enemy.second->SetTarget(player);
				}
			}
		}
		else
		{

            //get distance from player
            float distance = Math::Distance(enemy.second->Position(), enemy.second->GetTarget()->Position());
            if(distance > 380.0f)
            {
                enemy.second->SetTarget(nullptr);
                enemy.second->SetVelocity(sf::Vector2f(0.0f,0.0f));
                continue;
            }

			//move towards target
			sf::Vector2f direction = Math::Direction(enemy.second->Position(), enemy.second->GetTarget()->Position());
			enemy.second->SetVelocity(direction * enemy.second->BaseData().MovementSpeed);

			//shoot the target
			auto lastFire = enemy.second->GetLastFire();
			if(lastFire > 16)
			{
				auto bulletVelocity = direction * 350.0f;
                auto bullet = SpawnNewEntity("Bullet", enemy.second->Position(), bulletVelocity, 0, CollisionLayer::PROJECTILE_ENEMY);
                enemy.second->SetLastFire(0);
                LOG_INFO(enemy.second->GetLastFire());
            }
            else
            {
                enemy.second->SetLastFire(lastFire+1);
            }
		}
	}

	//update all entitie position
	for (const auto& entity : m_entities)
	{
		entity.second->ApplyVelocity(m_network->GetCurrentTickRate());
	}

	entityCollisions();
}

void WorldState::SetNetwork(Network& network)
{
	m_network = &network;
}

unsigned WorldState::GetSeed()
{
	return m_seed;
}

void WorldState::SpawnPlayer(Connection& connection)
{
	connection.m_entityID = entityIdCounter;
	//send all entities to the new player
	SpawnAllEntities();
	//spawn the player
	auto entity = SpawnNewEntity("Player", findValidSpawnPos(),sf::Vector2f(), connection.GetConnectionID(), CollisionLayer::NONE);
	m_players.insert(std::make_pair(entity->WorldID(), entity));
}

void WorldState::SpawnAllEntities()
{
	//TODO: only send messages once the client has generated the level

	std::vector<std::shared_ptr<Entity>> entities;
	for (auto& entity : m_entities)
	{
		entities.push_back(entity.second);
	}
	m_network->SendSpawnMessages(entities);
}

std::shared_ptr<Entity> WorldState::SpawnNewEntity(const std::string& entityName, sf::Vector2f position, sf::Vector2f velocity, unsigned int ownership, CollisionLayer layerOverride)
{
    std::unique_lock<std::mutex> l{m_entityMapMutex};

	unsigned int worldID = entityIdCounter++;
	//add to entity list
	auto entity = std::make_shared<Entity>(entityName ,worldID,position, velocity, ownership, layerOverride);
	m_entities.insert(std::make_pair(worldID, entity));

	if(m_network != nullptr)
		m_network->SendSpawnMessage(entity->WorldID(), entity->BaseData().EntityID, entity->Position(), entity->Velocity(), entity->OwnershipID(), layerOverride);
	//add to server collider list
	m_colliders.insert(entity->GetCollider());

	return entity;
}

void WorldState::DestroyEntity(unsigned worldID)
{
    std::unique_lock<std::mutex> l{m_entityMapMutex};

	if (m_entities.find(worldID) != m_entities.end())
	{
		auto& entity = m_entities.at(worldID);
		//remove collider
		m_colliders.erase(entity->GetCollider());
		m_entities.erase(worldID);
		m_enemies.erase(worldID);
		m_players.erase(worldID);
	}
}

void WorldState::SetEntityActive(unsigned worldID, bool active)
{

	if (m_entities.find(worldID) != m_entities.end())
	{
		auto& entity = m_entities.at(worldID);
		//remove collider
		entity->SetActive(active);
	}
}

void WorldState::SpawnEntity(int worldID)
{
    std::unique_lock<std::mutex> l{m_entityMapMutex};

	if (m_entities.find(worldID) != m_entities.end())
	{
		auto& entity = m_entities.at(worldID);
		m_network->SendSpawnMessage(entity->WorldID(), entity->BaseData().EntityID, entity->Position(), entity->Velocity(), entity->OwnershipID());
	}
}

void WorldState::MoveEntity(int worldID, sf::Vector2f newPosition, sf::Vector2f velocity)
{
	std::unique_lock<std::mutex> l{ m_entityMapMutex };

	//check if entity exists in the world state
	if(m_entities.find(worldID) != m_entities.end())
	{
		m_entities.at(worldID)->SetPosition(newPosition);
		m_entities.at(worldID)->SetVelocity(velocity);
	}
}

void WorldState::updateEntityVelocityFromClient(int worldID, sf::Vector2f velocity)
{
	std::unique_lock<std::mutex> l{ m_entityMapMutex };

	//check if entity exists in the world state
	if (m_entities.find(worldID) != m_entities.end())
	{
		m_entities.at(worldID)->SetVelocity(velocity);
		LOG_INFO("received new velocity from client");
	}
}

void WorldState::SetEntityHealth(unsigned worldID, float health, float maxHealth)
{
	std::unique_lock<std::mutex> l{ m_entityMapMutex };

	//check if entity exists in the world state
	if (m_entities.find(worldID) != m_entities.end())
	{
		auto& entity = m_entities.at(worldID);

		entity->SetMaxHealth(maxHealth);
		//Health can't be set higher than max health
		entity->SetHealth(std::min(entity->BaseData().MaxHealth, health));

		//set entity state to inactive if the entity is dead
		if (entity->BaseData().Health <= 0.0f)
			entity->SetActive(false);
		else
			entity->SetActive(true);

		//send back to clients
		m_network->SendHealthMessage(worldID, entity->BaseData().Health, entity->BaseData().MaxHealth);
	}
}

void WorldState::DamageEntity(unsigned worldID, float damage)
{
	std::unique_lock<std::mutex> l{ m_entityMapMutex };

	if (m_entities.find(worldID) != m_entities.end())
	{
		auto& entity = m_entities.at(worldID);
		//Damage server entity
		m_entities.at(worldID)->Damage(damage);
		//send new health to all clients
		m_network->SendHealthMessage(worldID, entity->BaseData().Health, entity->BaseData().MaxHealth);
	}
}

bool WorldState::IsColliding(unsigned worldID, unsigned otherWorldID) const
{

	if(m_entities.find(worldID) != m_entities.end() && m_entities.find(otherWorldID) != m_entities.end())
	{
		const auto& firstEntity = m_entities.at(worldID);
		const auto& otherEntity = m_entities.at(otherWorldID);;

		//check against actual server pos
		return m_entities.at(worldID)->GetCollider()->CheckCollision(*m_entities.at(otherWorldID)->GetCollider());
	}
	return false;
}

sf::Vector2f WorldState::findValidSpawnPos() const
{
	const std::vector<DungeonChunk*>&  chunks = m_dungeon->GetChunks();
	const std::vector<DungeonRoom>& rooms = chunks[0]->GetRooms();

	//Room tiles
	const std::vector<DungeonTile*>& tiles = rooms[0].GetTiles();

	//Get a random point within this room
	int index = Random::randInt(0, (int)tiles.size());
	sf::Vector2f chunkPos{(float)tiles[index]->x, (float)tiles[index]->y};
	return m_dungeon->ChunckToWorldSpace(0, chunkPos);
}

sf::Vector2f WorldState::findRandomPos() const
{
	int randomChunk = Random::randInt(0, 3);
	const std::vector<DungeonChunk*>&  chunks = m_dungeon->GetChunks();
	const std::vector<DungeonRoom>& rooms = chunks[randomChunk]->GetRooms();

	//Room tiles
	int randomRoom = Random::randInt(0, rooms.size()-1);
	const std::vector<DungeonTile*>& tiles = rooms[randomRoom].GetTiles();

	//Get a random point within this room
	int index = Random::randInt(0, (int)tiles.size()-1);
	sf::Vector2f chunkPos{ (float)tiles[index]->x, (float)tiles[index]->y };

	return m_dungeon->ChunckToWorldSpace(randomChunk, chunkPos);
}

void WorldState::SpawnEnemies()
{
    LOG_INFO("Spawning Enemies");
	constexpr int ENEMY_COUNT = 75;
	for (int i = 0; i < ENEMY_COUNT; ++i)
	{
		sf::Vector2f spawnPos = findRandomPos();
		std::shared_ptr<Entity> entity = SpawnNewEntity("Skeleton", spawnPos, sf::Vector2f(), 0, CollisionLayer::NONE);
		m_enemies.insert(std::make_pair(entity->WorldID(), entity));
	}
}

void WorldState::SpawnPickups()
{
	LOG_INFO("Spawning Pickups");
	constexpr int PICKUP_COUNT = 150;

	std::vector<std::string> pickupNames
	{
		"MultishotPickup",
		"ConeshotPickup",
		"SingleshotPickup",
		"FireratePickup",
		"HealthPickup"
	};

	for (int i = 0; i < PICKUP_COUNT; i++)
	{
		const std::string& pickupName = pickupNames[Random::randInt(0, pickupNames.size() - 1)];
		sf::Vector2f spawnPos = findRandomPos();
		std::shared_ptr<Entity> entity = SpawnNewEntity(pickupName, spawnPos, sf::Vector2f(), 0, CollisionLayer::PICKUP);
	}
}

std::shared_ptr<Entity> WorldState::GetClosestPlayer(const Entity& sourceEntity, float& distance) const
{
	if (m_players.empty())
		return nullptr;

	std::shared_ptr<Entity> closest;
	for (const auto& entity : m_players)
	{
		if (closest == nullptr)
		{
			closest = entity.second;
			distance = Math::Distance(closest->Position(), sourceEntity.Position());
			continue;
		}

		const float closestDistance = Math::Distance(entity.second->Position(), sourceEntity.Position());
		if(closestDistance < distance)
		{
			closest = entity.second;
			distance = closestDistance;
		}

	}
	return closest;
}

void WorldState::entityWorldCollision(Entity& entity)
{
	//Check world collisions
	const DungeonTile* startTile = m_dungeon->GetTileFromWorld(entity.Position());
	if (startTile == nullptr)
		return;
	//convert to chunk pos
	const sf::Vector2i intPos{ (int)std::floor(entity.Position().x),(int)std::floor(entity.Position().y) };
	const int chunkX = (intPos.x / 64) / 64;
	const int chunkY = (intPos.y / 64) / 64;

	for (int y = startTile->y - 1; y <= startTile->y + 1; ++y)
	{
		for (int x = startTile->x - 1; x <= startTile->x + 1; ++x)
		{
			const DungeonTile* tile = m_dungeon->GetTileFromChunk(chunkX, chunkY, x, y);

			if (tile == nullptr || tile->collider == nullptr)
				continue;
			Collider* collider = tile->collider;

			if (collider->CheckCollision(*entity.GetCollider()))
			{
				entity.SetPosition(entity.LastPosition());
				return;
			}
		}
	}
}

void WorldState::entityCollisions()
{
	for (auto& entity : m_entities)
	{
		if (!entity.second->BaseData().ServerCollisions)
		{
			continue;
		}

		entityWorldCollision(*entity.second);

        //check enemy collisions against other entities
        for(auto& otherEntity : m_entities)
        {
            if(entity == otherEntity || !otherEntity.second->BaseData().ServerCollisions)
            {
                continue;
            }

            auto entityCollider = otherEntity.second->GetCollider();
            if(entityCollider->CheckCollision(*entity.second->GetCollider()))
            {
                entity.second->SetPosition(entity.second->LastPosition());
                break;
            }
        }
	}
}


