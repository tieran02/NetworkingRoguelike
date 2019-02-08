#pragma once
#include <vector>

struct DungeonTile;
class DungeonChunk;

class DungeonRegion
{
public:
	DungeonRegion(const DungeonChunk& chunk, const std::vector<DungeonTile*>& tiles);
	const std::vector<DungeonTile*>& Tiles() { return m_tiles; }
private:
	int GetNeighbourCount(const DungeonChunk& chunk, const DungeonTile& tile);

	std::vector<DungeonTile*> m_tiles;
	std::vector<DungeonTile*> OuterTiles;
};
