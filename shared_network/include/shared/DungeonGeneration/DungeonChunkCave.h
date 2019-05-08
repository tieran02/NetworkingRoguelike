#pragma once
#include "DungeonChunk.h"

class DungeonChunkCave : public DungeonChunk
{
public:
	DungeonChunkCave(int x, int y) : DungeonChunk(x, y)
	{
	}

	void Generate() override;
};

