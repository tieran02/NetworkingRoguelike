#pragma once
#include <unordered_set>
#include "DungeonChunk.h"

namespace sf {
	class RenderWindow;
}

class Dungeon
{
public:
	Dungeon(int width, int height, int seed);
	~Dungeon();
	void Cleanup();

	void Generate();
	void Draw(sf::RenderWindow & window);

	const std::vector<DungeonChunk*>& GetChunks() const;
private:
	const unsigned int WIDTH{ 0 }, HEIGHT{ 0 };
	const int m_seed;
	std::vector<DungeonChunk*> m_chunks;
	void assignNeighbours(DungeonChunk* chunk);
	DungeonChunk* getChunk(int x, int y);
};

