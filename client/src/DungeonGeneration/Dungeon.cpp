#include <SFML/Graphics.hpp>
#include "Dungeon.h"
#include "CellularAutomata.h"
#include "DungeonChunkCave.h"
#include "Random.h"
#include <chrono>


Dungeon::Dungeon(int width, int height) : WIDTH(width), HEIGHT(height), m_chunks(width*height)
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
	unsigned long milliseconds_since_epoch =
		std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now().time_since_epoch()).count();
	Random::SetSeed(milliseconds_since_epoch);

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
		chunk->Draw(window);
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
