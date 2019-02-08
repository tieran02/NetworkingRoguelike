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
	const int WIDTH, HEIGHT, CHUNK_COUNT{9};
	std::vector<DungeonChunk*> m_chunks;
};

