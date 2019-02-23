#include "Random.h"
#include <ctime>
#include <chrono>

namespace Random 
{
	static thread_local std::mt19937 generator;

	int randInt(int min, int max)
	{
		std::uniform_int_distribution<int> distribution(min, max);
		int num = distribution(generator);
		return distribution(generator);
	}

	void SetSeed(int seed)
	{
		generator.seed(seed);
	}
}