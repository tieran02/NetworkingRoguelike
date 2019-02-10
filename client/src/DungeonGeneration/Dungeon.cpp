#include <SFML/Graphics.hpp>
#include "Dungeon.h"
#include "CellularAutomata.h"
#include "DungeonChunkCave.h"
#include <queue>
#include <iostream>


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

	connectChunks();
}



void Dungeon::Draw(sf::RenderWindow & window)
{
	for (auto& chunk : m_chunks)
	{
		chunk->Draw(window);
	}
}

void Dungeon::connectChunks()
{
	//use a minimal spanning tree to connect chunks together
	std::unordered_set<DungeonChunk*> visitedChunks;

	std::queue<DungeonChunk*> chunks;
	chunks.push(m_chunks[0]);
	visitedChunks.insert(m_chunks[0]);

	while (!chunks.empty())
	{
		DungeonChunk* currentChunk = chunks.front();
		chunks.pop();

		for (auto neighbour : currentChunk->GetNeighbours())
		{
			if (!currentChunk->GetConnections().empty())
				break;

			if(visitedChunks.find(neighbour) == visitedChunks.end())
			{
				//Connect chunks
				visitedChunks.insert(neighbour);
				chunks.push(neighbour);

				currentChunk->AddConnection(neighbour);

				connectChunk(currentChunk, neighbour);
			}
		}

	}
}

void Dungeon::connectChunk(DungeonChunk* from, DungeonChunk* to)
{
	//detect what direction
	int dx = from->GetX() - to->GetX();
	int dy = from->GetY() - to->GetY();

	if (dx == -1 && dy == 0) // Left
	{
		//from->createPassage(*from->m_rooms[0].GetEdgeTiles()[0], from->m_tiles[0][0]);
		//to->createPassage(*to->m_rooms[0].GetEdgeTiles()[0], to->m_tiles[16][0]);
	}
	else if (dx == 0 && dy == 1) // Top
	{

	}
	else if (dx == 1 && dy == 0) // right
	{

	}
	else if (dx == 0 && dy == -1) // bottom
	{

	}
	else if (dx == -1 && dy == 1) // Top Left
	{

	}
	else if (dx == 1 && dy == 1) // Top right
	{

	}
	else if (dx == -1 && dy == -1) // bottom Left
	{

	}
	else if (dx == 1 && dy == -1) // bottom right
	{

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
