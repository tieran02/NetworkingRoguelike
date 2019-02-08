#include "DungeonRoom.h"

DungeonRoom::DungeonRoom(const std::vector<DungeonTile*>& tiles, DungeonTile** map, unsigned int chunkSize) : tiles(tiles)
{
	for (auto& tile : tiles)
	{
		for (int y = tile->y - 1; y < tile->y + 1; ++y)
		{
			for (int x = tile->x - 1; x < tile->x + 1; ++x)
			{
				if(x == tile->x || y == tile->y && map[y][x].type == DungeonTileType::WALL)
				{
					edgeTiles.push_back(tile);
				}
			}
		}
	}
}
