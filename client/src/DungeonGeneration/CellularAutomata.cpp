#include "CellularAutomata.h"
#include <iostream>
#include "../Random.h"

CellularAutomata::CellularAutomata(unsigned int width, unsigned int height, int seed) : WIDTH(width), HEIGHT(height), SEED(seed)
{
	m_map.resize(height);
	for (unsigned int i = 0; i < height; ++i)
	{
		m_map[i].resize(width);
	}
}

CellularAutomata::~CellularAutomata()
{
}

void CellularAutomata::Generate(int smoothAmount, int fillPercent)
{
	randomFill(fillPercent);
	for (int i = 0; i < smoothAmount; i++)
	{
		m_map = SmoothCave(m_map);
	}
}

void CellularAutomata::Print()
{
	for (int i = 0; i < HEIGHT; ++i)
	{
		for (int j = 0; j < WIDTH; ++j)
		{
			if (m_map[i][j])
				std::cout << "0";
			else
				std::cout << " ";
		}
		std::cout << std::endl;
	}
}

const std::vector<std::vector<bool>>& CellularAutomata::GetCave() const
{
	return m_map;
}

void CellularAutomata::randomFill(int fillPercentage)
{
	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			//make all borders walls
			if (y == 0 || y == HEIGHT - 2 || x == 0 || x == WIDTH - 2)
				m_map[y][x] = true;
			else if (Random::Instance().RangeSeeded(1,100) < fillPercentage)
				m_map[y][x] = true;
			else
				m_map[y][x] = false;
		}
	}
}

std::vector<std::vector<bool>> CellularAutomata::SmoothCave(std::vector<std::vector<bool>>& oldMap)
{
	std::vector<std::vector<bool>> newMap;
	newMap.resize(HEIGHT);
	for (int i = 0; i < HEIGHT; ++i)
	{
		newMap[i].resize(WIDTH);
	}


	int birthThreshold = 5;
	int surviveThreshold = 4;
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			int surroundingWallTiles = GetSurroundingWallCount(y, x, oldMap);

			if (oldMap[y][x] == 0 && surroundingWallTiles >= birthThreshold)
			{
				newMap[y][x] = 1;
			}
			else if (oldMap[y][x] == 1 && surroundingWallTiles >= surviveThreshold)
			{
				newMap[y][x] = 1;
			}
			else
			{
				newMap[y][x] = 0;
			}
		}
	}
	return newMap;
}

int CellularAutomata::GetSurroundingWallCount(int gridX, int gridY, std::vector<std::vector<bool>>& map)
{
	int wallCount = 0;
	for (int y = gridY - 1; y <= gridY + 1; y++)
	{
		for (int x = gridX - 1; x <= gridX + 1; x++)
		{
			//if out of bounds
			if (y < 0 || y >= WIDTH || x < 0 || x >= HEIGHT)
			{
				wallCount++;
				continue;
			}

			if (y != gridX || x != gridY)
				wallCount += map[x][y];
		}
	}

	return wallCount;
}
