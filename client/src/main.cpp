#include <SFML/Graphics.hpp>
#include "shared/DungeonGeneration/Dungeon.h"
#include "Networking/ServerConnection.h"
#include "World.h"
#include "shared/Utility/Log.h"

int main()
{
	Log::Init();
	//World
	World world;
	//networking
	ServerConnection server_connection{4305, &world};
	server_connection.FindServer();
	server_connection.Connect();

	const int DUNGEON_SIZE{ 2 };
	world.Generate(&server_connection);

	int width{ 1280 }, height{ 720 };
	sf::RenderWindow window(sf::VideoMode(width, height), "SFML window");
	sf::View view(sf::FloatRect(0.0f, 0.0f, (float)width, (float)height));
	view.setCenter(64 / 2 * 32 * DUNGEON_SIZE, 64 / 2 * 32* DUNGEON_SIZE);
	view.zoom(-5.0);

	size_t size = sizeof(DungeonChunk);

	window.setView(view);
	while (window.isOpen())
	{
		// Event processing
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Request for closing the window
			if (event.type == sf::Event::Closed)
				window.close();
			else if(event.type == sf::Event::Resized)
			{
				sf::View view(sf::FloatRect(0.0f, 0.0f, (float)event.size.width, (float)event.size.height));
				view.setCenter(64 / 2 * 32 * DUNGEON_SIZE, 64 / 2 * 32 * DUNGEON_SIZE);
				view.zoom(-5);
				window.setView(view);
			}
			else if (event.type == sf::Event::GainedFocus)
				world.SetWindowFocused(true);
			else if (event.type == sf::Event::LostFocus)
				world.SetWindowFocused(false);
		}

		world.Update();
		server_connection.UpdateTick();
		window.clear();

		world.Draw(window);
		window.display();

		server_connection.PollMessages();

	}

	server_connection.Disconnect();
	return 0;
}
