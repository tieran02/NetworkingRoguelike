#include "Random.h"
#include <ctime>
#include <chrono>

int Random::RangeSeeded(int min, int max)
{
	gen = std::mt19937(m_seed);
	m_seed += 1;

	std::uniform_int_distribution<> dis(min, max);
	int num = dis(gen);
	return num;
}

int Random::Range(int min, int max)
{
	std::default_random_engine eng{ static_cast<long unsigned int>(time(0)) };
	std::uniform_int_distribution<> dis(min, max);
	return dis(eng);
}

void Random::SetSeed(long unsigned int seed)
{
	m_seed = seed;
	gen = std::mt19937(m_seed);
}
