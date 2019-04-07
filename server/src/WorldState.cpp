#include "WorldState.h"
#include "Network.h"
#include "shared/Random.h"


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
	m_dungeon = std::unique_ptr<Dungeon>(new Dungeon(2, 2, m_seed));
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
	SpawnEntity(0, findValidSpawnPos(), connection.GetConnectionID());
}

void WorldState::SpawnAllEntities()
{
	for (auto& entity : m_entities)
	{
		SpawnEntity(entity.second.EntityID, entity.second.Position, entity.second.OwnershipID);
	}
}

void WorldState::SpawnEntity(const int entityID, const sf::Vector2f position, unsigned int ownership)
{
	unsigned int worldID = entityIdCounter;
	m_network->SendSpawnMessage(worldID,entityID, position, ownership);
	//add to entity list
	Entity entity{ worldID,entityID,position,ownership };
	m_entities.insert(std::make_pair(worldID, entity));
	entityIdCounter++;
}

void WorldState::MoveEntity(int worldID, sf::Vector2f newPosition)
{
	//check if entity exists in the world state
	if(m_entities.find(worldID) != m_entities.end())
	{
		m_entities.at(worldID).Position = newPosition;
		//send pos to all clients
		m_network->SendMovementMessage(worldID, newPosition);
	}
}

sf::Vector2f WorldState::findValidSpawnPos() const
{
	const std::vector<DungeonChunk*>&  chunks = m_dungeon->GetChunks();
	const std::vector<DungeonRoom>& rooms = chunks[0]->GetRooms();

	//Room tiles
	const std::vector<DungeonTile*>& tiles = rooms[0].GetTiles();

	//Get a random point within this room
	int index = Random::randInt(0, tiles.size());
	sf::Vector2f chunkPos{(float)tiles[index]->x, (float)tiles[index]->y};
	return m_dungeon->ChunckToWorldSpace(0, chunkPos);
}
