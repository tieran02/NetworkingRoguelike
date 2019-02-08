#pragma once
#include "DungeonChunk.h"
#include <unordered_set>
#include <vector>

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

class DungeonChunkCave : public DungeonChunk
{
public:
	void Generate(int x, int y) override;
private:
	void connectRegions();
	std::vector<std::vector<DungeonTile*>> getRegions(DungeonTileType type);
	std::vector<DungeonTile*> getRegion(int x, int y);
	std::vector<DungeonTile*> getNeighbours(int startX, int startY);
	std::vector<DungeonTile*> getLine(const DungeonTile& from, const DungeonTile& to);
	void createPassage(const DungeonTile& tileA, const DungeonTile& tileB);
	void drawCircle(const DungeonTile& tile, int r);
};

