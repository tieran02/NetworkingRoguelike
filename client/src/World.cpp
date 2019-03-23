#include "World.h"
#include <iostream>
#include "Random.h"

World::World()
{
}


World::~World()
{
}

void World::Generate()
{
	m_dungeon = std::unique_ptr<Dungeon>(new Dungeon(2,2,m_seed));
	m_dungeon->Generate();
	SpawnPlayer();
}

void World::SetSeed(unsigned int seed)
{
	m_seed = seed;
}

void World::SpawnPlayer()
{
    FindSpawnPoint();
    std::cout << "Spawned Player @"  << m_spawnPos.x << "," << m_spawnPos.y << std::endl;
}

void World::Draw(sf::RenderWindow & window)
{
	m_dungeon->Draw(window);
}

void World::FindSpawnPoint()
{
    const std::vector<DungeonChunk*>&  chunks = m_dungeon->GetChunks();
    const std::vector<DungeonRoom>& rooms = chunks[0]->GetRooms();

    //Room tiles
    const std::vector<DungeonTile*>& tiles = rooms[0].GetTiles();

    //Get a random point within this room
    int index = Random::randInt(0, tiles.size());
    m_spawnPos.x = tiles[index]->x;
    m_spawnPos.y = tiles[index]->y;
}
