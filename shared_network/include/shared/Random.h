#pragma once
#include <random>

#if defined (_MSC_VER)  // Visual studio
#define thread_local __declspec( thread )
#elif defined (__GCC__) // GCC
#define thread_local __thread
#endif

namespace Random
{
	int randInt(int min, int max);
	void SetSeed(int seed);
}