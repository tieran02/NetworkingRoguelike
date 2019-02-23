#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include "Dungeon.h"
#include "CellularAutomata.h"
#include "DungeonChunkCave.h"
#include <queue>
#include <iostream>
#include "Random.h"
#include <chrono>
#include <ctime>


Dungeon::Dungeon(int chunkCount) : CHUNK_COUNT(chunkCount), m_chunks(chunkCount)
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

	//set seed
	Random::SetSeed(112337);

	//generate first chunk
	m_chunks[0] = new DungeonChunkCave(0,0);
	int chunkCount = 1;

	while (chunkCount != m_chunks.size())
	{
		generateNeighbourChunks(m_chunks[chunkCount - 1], chunkCount);
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
		chunk->Draw(window);
	}
}


void Dungeon::generateNeighbourChunks(const DungeonChunk* chunk, int& chunkCount)
{
	for (int y = chunk->GetY() - 1; y <= chunk->GetY() + 1; ++y)
	{
		for (int x = chunk->GetX() - 1; x <= chunk->GetX() + 1; ++x)
		{
			if(x == chunk->GetX() && y == chunk->GetY() )
				continue;

			if (chunkCount == CHUNK_COUNT)
				return;

			m_chunks[chunkCount++] = new DungeonChunkCave(x, y);
		}
	}
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
