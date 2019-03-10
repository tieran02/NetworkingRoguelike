#include <SFML/Graphics.hpp>
#include "DungeonGeneration/Dungeon.h"
#include "Networking/ServerConnection.h"
#include "Random.h"

int main()
{
	//networking
	ServerConnection server_connection{4305};
	server_connection.FindServer();
	server_connection.Connect();

	const int DUNGEON_SIZE{ 2 };
	Dungeon dungeon{ DUNGEON_SIZE,DUNGEON_SIZE, server_connection.GetSeed() };
	dungeon.Generate();

	int width{ 1280 }, height{ 720 };
	sf::RenderWindow window(sf::VideoMode(width, height), "SFML window");
	sf::View view(sf::FloatRect(0.0f, 0.0f, (float)width, (float)height));
	view.setCenter(64 / 2 * 16* DUNGEON_SIZE, 64 / 2 * 16* DUNGEON_SIZE);
	view.zoom(-2.5);

	size_t size = sizeof(DungeonChunk);

	window.setView(view);
	while (window.isOpen())
	{
		server_connection.PollMessages();


		// Event processing
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Request for closing the window
			if (event.type == sf::Event::Closed)
				window.close();
			if(event.type == sf::Event::Resized)
			{
				sf::View view(sf::FloatRect(0.0f, 0.0f, (float)event.size.width, (float)event.size.height));
				view.setCenter(64 / 2 * 16 * DUNGEON_SIZE, 64 / 2 * 16 * DUNGEON_SIZE);
				view.zoom(-2.5);
				window.setView(view);
			}
			if(event.type == sf::Event::KeyPressed)
			{
				dungeon.Generate();
			}
		}
		window.clear();

		dungeon.Draw(window);

		window.display();
	}

	server_connection.Disconnect();
	return 0;
}
