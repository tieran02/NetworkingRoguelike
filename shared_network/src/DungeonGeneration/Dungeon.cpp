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
	createDungeonColliders();
}



void Dungeon::Draw(sf::RenderWindow & window, std::shared_ptr<sf::Sprite> wallSprite, std::shared_ptr<sf::Sprite> floorSprite)
{
	for (auto& chunk : m_chunks)
	{
		chunk->Draw(window, m_tileSize,wallSprite,floorSprite);
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
	//sf::Vector2f chunkWorldPos{ chunk->GetX() * (float)chunkSize, chunk->GetY() * (float)chunkSize };
	sf::Vector2f chunkWorldPos{ chunk->GetX() * ((float)chunkSize-1), chunk->GetY() * ((float)chunkSize-1) };
	sf::Vector2f tileWorldPos = chunkWorldPos + sf::Vector2f{ chunckPos.x  , chunckPos.y };

	return tileWorldPos * (float)m_tileSize;
}

const DungeonTile* Dungeon::GetTileFromWorld(const sf::Vector2f& worldPos) const
{
	//convert to chunk pos
	const sf::Vector2i intPos{ (int)std::floor(worldPos.x),(int)std::floor(worldPos.y) };

	int chunkX = (intPos.x / m_tileSize) / (int)DungeonChunk::CHUNK_SIZE;
	int chunkY = (intPos.y / m_tileSize) / (int)DungeonChunk::CHUNK_SIZE;

	sf::Vector2f chunkWorldPos{ chunkX * ((float)DungeonChunk::CHUNK_SIZE - 1), chunkY * ((float)DungeonChunk::CHUNK_SIZE - 1) };
	sf::Vector2f tileWoldPos = (worldPos - chunkWorldPos) / (float)m_tileSize;

	int tileX = (intPos.x / m_tileSize) % (int)DungeonChunk::CHUNK_SIZE;
	int tileY = (intPos.y / m_tileSize) % (int)DungeonChunk::CHUNK_SIZE;

	if (chunkX < 0 || chunkY < 0 || tileX < 0 || tileY < 0 || chunkX >= WIDTH || chunkY >= HEIGHT)
		return nullptr;

	const unsigned int chunkOffset = (chunkX + chunkY * WIDTH);

	//getChunk
	return &m_chunks[chunkOffset]->m_tiles[tileY][tileX];
}

const DungeonTile* Dungeon::GetTileFromChunk(int chunkX, int chunkY, int tileX, int tileY)
{
	if (chunkX < 0 || chunkY < 0 || tileX < 0 || tileY < 0 || chunkX >= WIDTH || chunkY >= HEIGHT || tileX >= m_tileSize || tileY >= m_tileSize)
		return nullptr;

	const unsigned int chunkOffset = (chunkX + chunkY * WIDTH);

	return &m_chunks[chunkOffset]->m_tiles[tileY][tileX];
}

const DungeonTile** Dungeon::GetNeighboursOfTileFromWorld(const sf::Vector2f& worldPos)
{
	const DungeonTile* tiles[9]{nullptr};
	int index{ 0 };
	sf::Vector2i intPos = (sf::Vector2i)worldPos;
	for (int y = intPos.y - 1; y <= intPos.y +1 ; ++y)
	{
		for (int x = intPos.x - 1; x <= intPos.x + 1; ++x)
		{
			const DungeonTile* tile = GetTileFromWorld(sf::Vector2f{(float)x,(float)y});
			if (tile != nullptr)
				tiles[index++] = tile;
		}
	}
	return tiles;
}

std::vector<sf::RectangleShape> Dungeon::GetTileRectangles()
{
	std::vector<sf::RectangleShape> rects;
	for (int i = 0; i < m_chunks.size(); ++i)
	{
		auto& tiles = m_chunks[i]->m_tiles;

		for (auto& tile : m_chunks[i]->edgeTiles)
		{
			if (tile->type == DungeonTileType::EMPTY)
				continue;
			sf::RectangleShape rect;
			sf::Vector2f chunkPos = ChunckToWorldSpace(i, sf::Vector2f{ (float)m_chunks[i]->GetX() ,(float)m_chunks[i]->GetY() });
			sf::Vector2f worldPos = sf::Vector2f{ chunkPos.x + (tile->x * m_tileSize),chunkPos.y + (tile->y * m_tileSize) };
			rect.setPosition(worldPos.x, worldPos.y);
			rect.setSize(sf::Vector2f{ (float)m_tileSize,(float)m_tileSize });
			rects.push_back(rect);
		}


		//for (int y = 0; y < m_chunks[i]->GetSize(); ++y)
		//{
		//	for (int x = 0; x < m_chunks[i]->GetSize(); ++x)
		//	{
		//		auto& tile = tiles[y][x];
		//		if(tile.type == DungeonTileType::EMPTY)
		//			continue;
		//		sf::RectangleShape rect;
		//		sf::Vector2f chunkPos = ChunckToWorldSpace(i, sf::Vector2f{ (float)m_chunks[i]->GetX() ,(float)m_chunks[i]->GetY() });
		//		sf::Vector2f worldPos = sf::Vector2f{ chunkPos.x + (tile.x * m_tileSize),chunkPos.y + (tile.y * m_tileSize) };
		//		rect.setPosition(worldPos.x, worldPos.y);
		//		rect.setSize(sf::Vector2f{ (float)m_tileSize,(float)m_tileSize });
		//		rects.push_back(rect);
		//	}
		//}
	}

	return rects;
}

void Dungeon::createDungeonColliders()
{
	for (int i = 0; i < m_chunks.size(); ++i)
	{
		DungeonTile** tiles = m_chunks[i]->m_tiles;

		for (int y = 0; y < (int)DungeonChunk::CHUNK_SIZE; y++)
		{
			for (int x = 0; x < (int)DungeonChunk::CHUNK_SIZE; ++x)
			{
				DungeonTile* tile = &tiles[y][x];

				CollisionLayer layer = CollisionLayer::NONE;
				if (tile->type == DungeonTileType::WALL) {
					layer = CollisionLayer::WALL;

					sf::RectangleShape rect;
					sf::Vector2f chunkPos = ChunckToWorldSpace(i, sf::Vector2f{ (float)m_chunks[i]->GetX() ,(float)m_chunks[i]->GetY() });
					sf::Vector2f worldPos = sf::Vector2f{ chunkPos.x + (tile->x * m_tileSize),chunkPos.y + (tile->y * m_tileSize) };
					rect.setPosition(worldPos.x, worldPos.y);
					rect.setSize(sf::Vector2f{ (float)m_tileSize,(float)m_tileSize });
					tile->collider = new Collider(rect, layer);
				}
			}
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
