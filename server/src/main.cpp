
#include "Network.h"
#include "WorldState.h"
#include <iostream>
#include "shared/Utility/Log.h"

int main()
{
	Log::Init();

	//load entity list
	EntityDataManager::Instance();

	LOG_INFO("Enter level seed: ");
	unsigned int seed = 0;
	std::cin >> seed;

	WorldState world{ seed };
	world.GenerateWorld();

	Network network(world,8305);
	world.SetNetwork(network);
	network.Start();

    return 0;
}
