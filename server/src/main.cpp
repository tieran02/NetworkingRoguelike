
#include "Network.h"
#include "WorldState.h"
#include <iostream>

int main()
{
	std::cout << "Enter level seed: ";
	unsigned int seed = 0;
	std::cin >> seed;

	WorldState world{ seed };
	world.GenerateWorld();

	Network network(world,4305);
	world.SetNetwork(network);
	network.Start();

    return 0;
}
