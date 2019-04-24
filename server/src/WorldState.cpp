#include "WorldState.h"
#include "Network.h"
#include "shared/Random.h"
#include <memory>


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
	m_dungeon = std::unique_ptr<Dungeon>(new Dungeon(2, 2,64, m_seed));
	m_dungeon->Generate();

	//Set dungeon colliders
	auto rects = m_dungeon->GetTileRectangles();
	for (auto& rect : rects)
	{
		auto collider = std::make_shared<Collider>(rect, CollisionLayer::WALL);
		m_colliders.insert(collider);
	}
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
	SpawnNewEntity("Player", findValidSpawnPos(),sf::Vector2f(), connection.GetConnectionID(), CollisionLayer::NONE);
}

void WorldState::SpawnAllEntities()
{
	std::shared_lock<std::shared_mutex> lock{ m_entityMapMutex };
	//TODO: only send messages once the client has generated the level

	for (auto& entity : m_entities)
	{
		SpawnEntity(entity.first);
	}
}

void WorldState::SpawnNewEntity(const std::string& entityName, sf::Vector2f position, sf::Vector2f velocity, unsigned int ownership, CollisionLayer layerOverride)
{
	std::unique_lock<std::shared_mutex> lock{ m_entityMapMutex };

	unsigned int worldID = entityIdCounter++;
	//add to entity list
	auto entity = std::make_shared<Entity>(entityName ,worldID, position, velocity, ownership, layerOverride);
	m_entities.insert(std::make_pair(worldID, entity));
	m_network->SendSpawnMessage(entity->WorldID(), entity->BaseData().EntityID, entity->Position(), entity->Velocity(), entity->OwnershipID(), layerOverride);
	//add to server collider list
	m_colliders.insert(entity->GetCollider());
}

void WorldState::DestroyEntity(unsigned worldID)
{
	std::unique_lock<std::shared_mutex> lock{ m_entityMapMutex };
	
	if (m_entities.find(worldID) != m_entities.end())
	{
		auto& entity = m_entities.at(worldID);
		//remove collider
		m_colliders.erase(entity->GetCollider());
		m_entities.erase(worldID);
	}
}

void WorldState::SetEntityActive(unsigned worldID, bool active)
{
	std::unique_lock<std::shared_mutex> lock{ m_entityMapMutex };

	if (m_entities.find(worldID) != m_entities.end())
	{
		auto& entity = m_entities.at(worldID);
		//remove collider
		entity->SetActive(active);
	}
}

void WorldState::SpawnEntity(int worldID)
{
	std::shared_lock<std::shared_mutex> lock{ m_entityMapMutex };

	if (m_entities.find(worldID) != m_entities.end())
	{
		auto& entity = m_entities.at(worldID);
		m_network->SendSpawnMessage(entity->WorldID(), entity->BaseData().EntityID, entity->Position(), entity->Velocity(), entity->OwnershipID());
	}
}

void WorldState::MoveEntity(int worldID, sf::Vector2f newPosition, sf::Vector2f velocity)
{
	std::shared_lock<std::shared_mutex> lock{ m_entityMapMutex };

	//check if entity exists in the world state
	if(m_entities.find(worldID) != m_entities.end())
	{
		m_entities.at(worldID)->SetPosition(newPosition);
		m_entities.at(worldID)->SetVelocity(velocity);
	}
}

void WorldState::SetEntityHealth(unsigned worldID, float health, float maxHealth)
{
	std::shared_lock<std::shared_mutex> lock{ m_entityMapMutex };

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
