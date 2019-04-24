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
	SpawnNewEntity(0, findValidSpawnPos(),sf::Vector2f(), connection.GetConnectionID());
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

void WorldState::SpawnNewEntity(const int entityID, sf::Vector2f position, sf::Vector2f velocity, unsigned int ownership)
{
	std::unique_lock<std::shared_mutex> lock{ m_entityMapMutex };

	unsigned int worldID = entityIdCounter++;
	//add to entity list
	auto entity = std::make_shared<Entity>(worldID, entityID, position, velocity, ownership);
	m_entities.insert(std::make_pair(worldID, entity));
	m_network->SendSpawnMessage(entity->WorldID, entity->EntityID, entity->Position, entity->Velocity, entity->OwnershipID);
}

void WorldState::SpawnEntity(int worldID)
{
	std::shared_lock<std::shared_mutex> lock{ m_entityMapMutex };

	if (m_entities.find(worldID) != m_entities.end())
	{
		auto& entity = m_entities.at(worldID);
		m_network->SendSpawnMessage(entity->WorldID, entity->EntityID, entity->Position, entity->Velocity, entity->OwnershipID);
	}
}

void WorldState::MoveEntity(int worldID, sf::Vector2f newPosition, sf::Vector2f velocity)
{
	std::shared_lock<std::shared_mutex> lock{ m_entityMapMutex };

	//check if entity exists in the world state
	if(m_entities.find(worldID) != m_entities.end())
	{
		m_entities.at(worldID)->Position = newPosition;
		m_entities.at(worldID)->Velocity = velocity;
	}
}

void WorldState::SetEntityHealth(unsigned worldID, float health, float maxHealth)
{
	std::shared_lock<std::shared_mutex> lock{ m_entityMapMutex };

	//check if entity exists in the world state
	if (m_entities.find(worldID) != m_entities.end())
	{
		auto& entity = m_entities.at(worldID);

		entity->MaxHealth = maxHealth;
		//Health can't be set higher than max health
		entity->Health = std::min(entity->MaxHealth, health);

		//set entity state to inactive if the entity is dead
		if (entity->Health <= 0.0f)
			entity->IsActive = false;
		else
			entity->IsActive = true;

		//send back to clients
		m_network->SendHealthMessage(worldID, entity->Health, entity->MaxHealth);
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
