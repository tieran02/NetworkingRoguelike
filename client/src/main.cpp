#include <SFML/Graphics.hpp>
#include "shared/DungeonGeneration/Dungeon.h"
#include "Networking/ServerConnection.h"
#include "World.h"
#include "shared/Utility/Log.h"
#include "Graphics/SpriteManager.h"

int main()
{
	unsigned int width{ 1280 }, height{ 720 };

	Log::Init();

	LOG_INFO("Loading Resouces...");
	SpriteManager::Instance().LoadTexture("player", "player.png");
	SpriteManager::Instance().LoadTexture("wall", "wall.png");
	SpriteManager::Instance().LoadTexture("floor", "floor.png");
	SpriteManager::Instance().LoadTexture("bullet", "bullet.png");
	SpriteManager::Instance().LoadTexture("skeleton", "skeleton.png");
	LOG_INFO("Resouces Loaded");

	//Create window
	sf::RenderWindow window;

	//World
	World world{ window };
	world.SetWindowSize(sf::Vector2u(width, height));

	//networking
	ServerConnection server_connection{ 4305, &world };
	server_connection.FindServer();
	server_connection.Connect();
	//Generate World
	world.Generate(&server_connection);

	window.create(sf::VideoMode(width, height), "SFML window");

	float lastTime{ 0.0f };
	float deltaTime{ 0.0f };
	while (window.isOpen())
	{
		float currentTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		// Event processing
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Request for closing the window
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				width = event.size.width;
				height = event.size.height;
				world.SetWindowSize(sf::Vector2u(width, height));
			}
			else if (event.type == sf::Event::GainedFocus)
				world.SetWindowFocused(true);
			else if (event.type == sf::Event::LostFocus)
				world.SetWindowFocused(false);
		}

		world.Update(deltaTime);
		server_connection.UpdateTick();
		window.clear(sf::Color{ 80,64,64,255 });

		world.Draw(window);
		window.display();

		server_connection.PollMessages();
		deltaTime = currentTime - lastTime;
		//LOG_INFO("FPS = " + std::to_string(1.0f / (currentTime - lastTime)));

		lastTime = currentTime;
	}

	server_connection.Disconnect();
	return 0;
}
