#include <SFML/Graphics.hpp>

#include "Dungeon.h"
#include "CellularAutomata.h"
#include "DungeonChunkCave.h"


Dungeon::Dungeon(int width, int height) : WIDTH(width), HEIGHT(height)
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
	m_chunks.resize(CHUNK_COUNT);

	int x{ 0 }, y{ 0 };
	for (int i = 0; i < CHUNK_COUNT; ++i)
	{
		m_chunks[i] = new DungeonChunkCave();
		m_chunks[i]->Generate(x,y);

		//TODO make the x and y non hardcoded
		x++;
		if(x >= 3) 
		{
			x = 0;
			y++;
		}
	}
}

void Dungeon::Draw(sf::RenderWindow & window)
{
	for (auto& chunk : m_chunks)
	{
		chunk->Draw(window);
	}
}
