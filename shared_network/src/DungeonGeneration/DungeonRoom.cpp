#include <algorithm>
#include "DungeonGeneration/DungeonRoom.h"
#include "DungeonGeneration/Dungeon.h"

DungeonRoom::DungeonRoom(const std::vector<DungeonTile*>& tiles, const std::vector<DungeonTile*>& edgeWalls, DungeonTile** map, unsigned int chunkSize) : tiles(tiles), edgeTiles(edgeWalls)
{

}

DungeonRoom::~DungeonRoom()
{
}

bool DungeonRoom::IsConnected(const DungeonRoom& otherRoom) const
{
	return std::find(connectedRooms.begin(), connectedRooms.end(), &otherRoom) != connectedRooms.end();
}

void DungeonRoom::ConnectRooms(DungeonRoom& roomA, DungeonRoom& roomB)
{
	roomA.connectedRooms.push_back(&roomB);
	roomB.connectedRooms.push_back(&roomA);
}
