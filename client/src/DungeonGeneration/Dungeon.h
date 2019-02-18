#pragma once
#include <unordered_set>
#include "DungeonChunk.h"

namespace sf {
	class RenderWindow;
}

class Dungeon
{
public:
	Dungeon(int chunkCount = 9);
	~Dungeon();
	void Cleanup();

	void Generate();
	void Draw(sf::RenderWindow & window);
private:
	const int CHUNK_COUNT;
	std::vector<DungeonChunk*> m_chunks;
	void generateNeighbourChunks(const DungeonChunk* chunk, int& chunkCount);
	void assignNeighbours(DungeonChunk* chunk);
	DungeonChunk* getChunk(int x, int y);
};

