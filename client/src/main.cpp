#include <SFML/Graphics.hpp>

int main()
{
	const int width{ 1280 }, height{ 720 };
	
	sf::RenderWindow window(sf::VideoMode(width, height), "SFML window");

	while (window.isOpen())
	{
		// Event processing
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Request for closing the window
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();

		window.display();
	}

	return 0;
}
