#include "DungeonChunk.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <unordered_set>

int sign(int val) {
	return ((0) < val) - (val < (0));
}

DungeonChunk::DungeonChunk()
{
	//Allocate dungeon tiles
	m_tiles = new DungeonTile*[CHUNK_SIZE];
	for (int i = 0; i < (int)CHUNK_SIZE; ++i)
		m_tiles[i] = new DungeonTile[CHUNK_SIZE];

	int id{ 0 };
	for (int y = 0; y < (int)CHUNK_SIZE; y++)
	{
		for (int x = 0; x < (int)CHUNK_SIZE; x++)
		{
			m_tiles[y][x].x = x;
			m_tiles[y][x].y = y;
			m_tiles[y][x].id = id++;
			m_tiles[y][x].type = DungeonTileType::EMPTY;
		}
	}
}

DungeonChunk::~DungeonChunk()
{
	for (int i = 0; i < (int)CHUNK_SIZE; ++i) {
		delete[] m_tiles[i];
	}
	delete[] m_tiles;
}

void DungeonChunk::Draw(sf::RenderWindow& window)
{
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(16, 16));
	rectangle.setFillColor(sf::Color::Blue);

	sf::Vector2i offset{ chunkX * (int)CHUNK_SIZE * 16, chunkY * (int)CHUNK_SIZE * 16 };

	for (int y = 0; y < (int)CHUNK_SIZE; ++y)
	{
		for (int x = 0; x < (int)CHUNK_SIZE; ++x)
		{
			sf::Vector2f position = sf::Vector2f((x*16.0f), (y*16.0f)) + (sf::Vector2f)offset;
			rectangle.setPosition(position);

			switch (m_tiles[y][x].type)
			{
			case DungeonTileType::EMPTY:
				break;
			case DungeonTileType::WALL:
				window.draw(rectangle);
				break;
			default: 
				break;
			}
		}
	}
}

bool DungeonChunk::InBounds(int x, int y) const
{
	return x >= 0 && x < (int)CHUNK_SIZE && y >= 0 && y < (int)CHUNK_SIZE;
}
