#pragma once
#include <vector>

namespace sf {
	class RenderWindow;
}

enum class DungeonTileType
{
	EMPTY,
	WALL
};

struct DungeonTile
{
	enum class Type
	{
		EMPTY,
		WALL
	};

	int x{ 0 }, y{ 0 }, id{ 0 };
	DungeonTileType type{DungeonTileType::EMPTY};
};

class DungeonChunk
{
public:
	DungeonChunk();
	~DungeonChunk();

	virtual void Generate(int x, int y) = 0;
	void Draw(sf::RenderWindow& window);
	bool InBounds(int x, int y) const;
protected:
	const unsigned int CHUNK_SIZE{ 32 };
	int chunkX, chunkY;
	DungeonTile** m_tiles;

};
