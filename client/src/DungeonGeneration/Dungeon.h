#pragma once
#include <unordered_set>
#include "DungeonChunk.h"

namespace sf {
	class RenderWindow;
}

class Dungeon
{
public:
	Dungeon(int width, int height);
	~Dungeon();
	void Cleanup();

	void Generate();
	void Draw(sf::RenderWindow & window);
private:
	const unsigned int WIDTH{ 0 }, HEIGHT{ 0 };
	std::vector<DungeonChunk*> m_chunks;
	void assignNeighbours(DungeonChunk* chunk);
	DungeonChunk* getChunk(int x, int y);
};

