#include <iostream>
#include <complex>
#include "DungeonGeneration/DungeonChunkCave.h"
#include "DungeonGeneration/CellularAutomata.h"
#include "Utility/Log.h"

void DungeonChunkCave::Generate()
{
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

	processMap();
	LOG_TRACE("Generated Cave Chunk");
}
