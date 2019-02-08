#include "DungeonChunkCave.h"
#include "CellularAutomata.h"
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <complex>

void DungeonChunkCave::Generate(int x, int y)
{
	chunkX = x;
	chunkY = y;

	CellularAutomata cellular_automata{ CHUNK_SIZE,CHUNK_SIZE };
	cellular_automata.Generate(5, 38);

	//Copy the data from the cellular automata to tilemap
	auto map = cellular_automata.GetCave();
	for (int y = 0; y < (int)CHUNK_SIZE; ++y)
	{
		for (int x = 0; x < (int)CHUNK_SIZE; ++x)
		{
			DungeonTileType type = DungeonTileType::EMPTY;
			if(map[y][x])
				type = DungeonTileType::WALL;

			m_tiles[y][x].type = type;
		}
	}

	connectRegions();
	std::cout << "Generated Cave Chunk\n";
}

void DungeonChunkCave::connectRegions()
{
	auto regions = getRegions(DungeonTileType::EMPTY);

	//Get the biggest region which would be the main room
	int biggestRegionIndex = 0;
	int regionSize = 0;
	for (int i = 0; i < regions.size(); ++i)
	{
		if((int)regions[i].size() > regionSize)
		{
			regionSize = (int)regions[i].size();
			biggestRegionIndex = i;
		}
	}

	for (auto region : regions)
	{
		if(region == regions[biggestRegionIndex])
			continue;

		int randomTileIndex = std::rand() % region.size() + 1;
		createPassage(*region[0], *regions[biggestRegionIndex][0]);
	}
}


std::vector<std::vector<DungeonTile*>> DungeonChunkCave::getRegions(DungeonTileType type)
{
	std::unordered_set<int> visitedNodes;

	std::vector<std::vector<DungeonTile*>> regions;

	for (int y = 0; y < (int)CHUNK_SIZE; y++)
	{
		for (int x = 0; x < (int)CHUNK_SIZE; x++)
		{
			if (visitedNodes.find(m_tiles[y][x].id) == visitedNodes.end() && m_tiles[y][x].type == type)
			{
				auto tiles = getRegion(x, y);
				regions.push_back(tiles);

				for (auto& tile : tiles)
				{
					visitedNodes.insert(tile->id);
				}
			}
		}
	}
    std::cout << "Region count " << regions.size() << std::endl;
	return regions;
}

std::vector<DungeonTile*> DungeonChunkCave::getRegion(int startX, int startY)
{
	std::vector<DungeonTile*> tiles;

	std::unordered_set<int> visitedNodes;

	DungeonTileType tileType = m_tiles[startY][startX].type;

	std::queue<DungeonTile*> queue;
	queue.push(&m_tiles[startY][startX]);
	visitedNodes.insert(m_tiles[startY][startX].id);

	while (!queue.empty())
	{
		DungeonTile* tile = queue.front();
		queue.pop();
		tiles.push_back(tile);

		for (auto& neighbour : getNeighbours(tile->x, tile->y))
		{
			if (neighbour->type == tileType && visitedNodes.find(neighbour->id) == visitedNodes.end()) {
				visitedNodes.insert(neighbour->id);
				queue.push(neighbour);
			}
		}
	}

	return tiles;
}


std::vector<DungeonTile*> DungeonChunkCave::getNeighbours(int startX, int startY)
{
	std::vector<DungeonTile*> tiles;
	for (int y = startY - 1; y <= startY + 1; ++y)
	{
		for (int x = startX - 1; x <= startX + 1; ++x)
		{
			if (InBounds(x, y) && (y == startY || x == startX))
			{
				tiles.push_back(&m_tiles[y][x]);
			}
		}
	}
	return tiles;
}

std::vector<DungeonTile*> DungeonChunkCave::getLine(const DungeonTile& from, const DungeonTile& to)
{
	std::vector<DungeonTile*> line;

	int x = from.x;
	int y = from.y;

	int dx = to.x - from.x;
	int dy = to.y - from.y;

	bool inverted = false;

	int step = sign(dx);
	int gradientStep = sign(dy);

	int longest = abs(dx);
	int shortest = abs(dy);

	if(longest < shortest)
	{
		inverted = true;
		longest = shortest;
		shortest = abs(dx);

		step = sign(dy);
		gradientStep = sign(dx);
	}

	int gradiantAccumlation = longest / 2;
	for (int i = 0; i < longest; ++i)
	{
		line.push_back(&m_tiles[y][x]);

		if (inverted)
			y += step;
		else
			x += step;

		gradiantAccumlation += shortest;
		if(gradiantAccumlation >= longest)
		{
			if (inverted)
				x += gradientStep;
			else
				y += gradientStep;

			gradiantAccumlation -= longest;
		}
	}

	return line;
}

void DungeonChunkCave::createPassage(const DungeonTile& tileA, const DungeonTile& tileB)
{
	auto line = getLine(tileA, tileB);

	//create passage by making a cirlce at each tile
	for (auto& tile : line)
	{
		drawCircle(*tile, 1);
	}
}

void DungeonChunkCave::drawCircle(const DungeonTile& tile, int r)
{
	for (int x = -r; x < r; ++x)
	{
		for (int y = -r; y < r; ++y)
		{
			if (x*x + y * y <= r * r)
			{
				int realX = tile.x + x;
				int realY = tile.y + y;
				if(InBounds(realX, realY) && realX >= 1 && realX < (int)CHUNK_SIZE - 1 && realY >= 1 && realY < (int)CHUNK_SIZE - 1)
				{
					m_tiles[realY][realX].type = DungeonTileType::EMPTY;
				}
			}
		}
	}
}



