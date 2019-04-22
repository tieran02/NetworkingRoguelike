#pragma once
#include <vector>
#include "DungeonRoom.h"
#include <SFML/Graphics.hpp>
#include <unordered_set>

class Dungeon;

namespace sf {
	class RenderWindow;
}

enum class DungeonTileType
{
	EMPTY,
	WALL
};

class DungeonTile
{
public:
	int x{ 0 }, y{ 0 }, id{ 0 };
	DungeonTileType type{DungeonTileType::EMPTY};
};

class DungeonChunk
{
	friend Dungeon;
public:
	DungeonChunk(int x, int y);
	virtual ~DungeonChunk();

	virtual void Generate() = 0;
	void Draw(sf::RenderWindow& window, int tileSize, std::shared_ptr<sf::Sprite> wallSprite, std::shared_ptr<sf::Sprite> floorSprite);
	bool InBounds(int x, int y) const;
	int GetX() const { return chunkX; }
	int GetY() const { return chunkY; }
	unsigned int GetSize() const {return CHUNK_SIZE;}
	void AddNeighbour(DungeonChunk* chunk);
	const std::vector<DungeonChunk*>& GetNeighbours() const { return m_neighbourChunks; }
	const std::vector<DungeonRoom>& GetRooms() const;
protected:
	const unsigned int CHUNK_SIZE{ 64 };
	int chunkX, chunkY;
	DungeonTile** m_tiles;
	std::vector<DungeonRoom> m_rooms;
	std::vector<DungeonChunk*> m_neighbourChunks;
	std::unordered_set<DungeonTile*> edgeTiles;
	void processMap();
	void findEdgeTiles();
private:
	void connectRooms();
	void connectRoom(DungeonRoom& roomA, DungeonRoom& roomB);
	int getDistanceBetweenRooms(DungeonRoom& roomA, DungeonRoom& roomB);;
	void detectRooms();
	DungeonRoom getRoom(int x, int y);
	std::vector<DungeonTile*> getNeighbours(int startX, int startY);
	std::vector<DungeonTile*> getLine(const DungeonTile& from, const DungeonTile& to);
	void createPassage(const DungeonTile& tileA, const DungeonTile& tileB);
	void drawCircle(const DungeonTile& tile, int r);

	//debug colour rooms
	std::vector<sf::Color> m_roomColours;
};
