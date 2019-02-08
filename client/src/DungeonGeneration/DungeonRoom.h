#pragma once
#include "Dungeon.h"

class DungeonRoom
{
public:
	DungeonRoom(const std::vector<DungeonTile*>& tiles, DungeonTile** map, unsigned int chunkSize);
	~DungeonRoom();

	int RoomSize() const { return tiles.size(); }
private:
	std::vector<DungeonTile*> tiles;
	std::vector<DungeonTile*> edgeTiles;
	std::vector<DungeonRoom*> connectedRooms;
};
