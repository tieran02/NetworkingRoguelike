#pragma once
#include <unordered_set>
#include "DungeonChunk.h"
#include <SFML/Graphics.hpp>

namespace sf {
	class RenderWindow;
}

class Dungeon
{
public:
	Dungeon(int width, int height, int tileSize, int seed);
	~Dungeon();
	void Cleanup();

	void Generate();
	void Draw(sf::RenderWindow & window, std::shared_ptr<sf::Sprite> wallSprite, std::shared_ptr<sf::Sprite> floorSprite);

	const std::vector<DungeonChunk*>& GetChunks() const;
	sf::Vector2f ChunckToWorldSpace(int chunkID, sf::Vector2f chunckPos) const;

	std::vector<sf::RectangleShape> GetTileRectangles();

private:
	const unsigned int WIDTH{ 0 }, HEIGHT{ 0 };
	const int m_seed;
	const int m_tileSize;
	std::vector<DungeonChunk*> m_chunks;
	void assignNeighbours(DungeonChunk* chunk);
	DungeonChunk* getChunk(int x, int y);
};

