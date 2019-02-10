#pragma once
#include <random>

class Random
{
public:
	static Random& Instance()
	{
		static Random instance;
		instance.SetSeed(0);
		return instance;
	}

	Random(Random const&) = delete;
	void operator=(Random const&) = delete;
private:
	Random() {}

public:
	int RangeSeeded(int min, int max);
	int Range(int min, int max);
	void SetSeed(long unsigned int);
private:
	long unsigned int m_seed{0};
	std::random_device rd;
	std::mt19937 gen;
};

