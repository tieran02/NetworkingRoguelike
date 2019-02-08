#include <SFML/Graphics.hpp>
#include "DungeonGeneration/Dungeon.h"

int main()
{
	Dungeon dungeon{ 32,32 };
	dungeon.Generate();

	int width{ 1280 }, height{ 720 };
	sf::RenderWindow window(sf::VideoMode(width, height), "SFML window");
	sf::View view(sf::FloatRect(0.0f, 0.0f, (float)width, (float)height));
	view.setCenter(32 * 1.5 * 16, 32 * 1.5 * 16);
	view.zoom(-2.5);

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
			if(event.type == sf::Event::Resized)
			{
				sf::View view(sf::FloatRect(0.0f, 0.0f, (float)event.size.width, (float)event.size.height));
				view.setCenter(32 * 1.5 * 16, 32 * 1.5 * 16);
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

	return 0;
}
