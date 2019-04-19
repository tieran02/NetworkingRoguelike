#include <SFML/Graphics.hpp>
#include "Random.h"
#include <chrono>
#include "DungeonGeneration/Dungeon.h"
#include "DungeonGeneration/DungeonChunkCave.h"


Dungeon::Dungeon(int width, int height, int tileSize, int seed) : WIDTH(width), HEIGHT(height),m_tileSize(tileSize), m_chunks(width*height), m_seed(seed)
{

}


Dungeon::~Dungeon()
{
	Cleanup();
}

void Dungeon::Cleanup()
{
	for (auto chunk : m_chunks)
	{
		delete chunk;
	}
}

void Dungeon::Generate()
{
	Cleanup();

	Random::SetSeed(m_seed);

	int index = 0;
	for (unsigned int y = 0; y < HEIGHT; ++y)
	{
		for (unsigned int x = 0; x < WIDTH; x++)
		{
			m_chunks[index] = new DungeonChunkCave(x, y);
			index++;
		}
	}

	for (auto chunk : m_chunks)
	{
		assignNeighbours(chunk);
		chunk->Generate();
	}
}



void Dungeon::Draw(sf::RenderWindow & window)
{
	for (auto& chunk : m_chunks)
	{
		chunk->Draw(window, m_tileSize);
	}
}


const std::vector<DungeonChunk*>& Dungeon::GetChunks() const
{
    return m_chunks;
}


sf::Vector2f Dungeon::ChunckToWorldSpace(int chunkID, sf::Vector2f chunckPos) const
{
	sf::Vector2f pos;
	auto& chunk = m_chunks[chunkID];
	unsigned int chunkSize = chunk->GetSize();
	sf::Vector2f chunkWorldPos{ chunk->GetX() * (float)chunkSize, chunk->GetY() * (float)chunkSize };

	pos = chunckPos + chunkWorldPos;
	return pos * 16.0f; //16.0f is the tile size
}

void Dungeon::assignNeighbours(DungeonChunk* chunk)
{
	for (int y = chunk->GetY() - 1; y <= chunk->GetY() + 1; ++y)
	{
		for (int x = chunk->GetX() - 1; x <= chunk->GetX() + 1; ++x)
		{
			if (x == chunk->GetX() && y == chunk->GetY())
				continue;

			DungeonChunk* neighbourChunk = getChunk(x, y);
			if(neighbourChunk == nullptr)
				continue;

			chunk->AddNeighbour(neighbourChunk);
		}
	}
}

DungeonChunk* Dungeon::getChunk(int x, int y)
{
	for (auto chunk : m_chunks)
	{
		if (chunk->GetX() == x && chunk->GetY() == y)
			return chunk;
	}
	return nullptr;
}
