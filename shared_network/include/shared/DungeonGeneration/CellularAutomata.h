#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>

class CellularAutomata
{
public:
	CellularAutomata(unsigned int width, unsigned int height, int seed = 0);
	~CellularAutomata();

	void Generate(int smoothAmount, int fillPercent);
	void Print();

	const std::vector<std::vector<bool>>& GetCave() const;

private:
	const int WIDTH, HEIGHT, SEED;

	std::vector<std::vector<bool>> m_map;

	void randomFill(int fillPercentage);
	std::vector<std::vector<bool>> SmoothCave(std::vector<std::vector<bool>>& oldMap);
	int GetSurroundingWallCount(int gridX, int gridY, std::vector<std::vector<bool>>& map);
};

