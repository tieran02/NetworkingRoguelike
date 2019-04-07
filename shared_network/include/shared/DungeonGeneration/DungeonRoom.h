#pragma once
#include <vector>

class DungeonChunk;
class DungeonTile;

class DungeonRoom
{
	friend DungeonChunk;
public:
	DungeonRoom(const std::vector<DungeonTile*>& tiles, DungeonTile** map, unsigned int chunkSize);
	~DungeonRoom();

	int RoomSize() const { return (int)tiles.size(); }
	const std::vector<DungeonTile*>& GetTiles() const { return tiles; }
	const std::vector<DungeonTile*>& GetEdgeTiles() const { return edgeTiles; }
	bool IsConnected(const DungeonRoom& otherRoom) const;
	static void ConnectRooms(DungeonRoom& roomA, DungeonRoom& roomB);


	bool operator < (const DungeonRoom& other) const
	{
		return (tiles.size() < other.tiles.size());
	}
	bool operator > (const DungeonRoom& other) const
	{
		return (tiles.size() > other.tiles.size());
	}
private:
	std::vector<DungeonTile*> tiles;
	std::vector<DungeonTile*> edgeTiles;
	std::vector<DungeonRoom*> connectedRooms;
};
