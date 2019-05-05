#include <SFML/Graphics.hpp>
#include "shared/DungeonGeneration/Dungeon.h"
#include "Networking/ServerConnection.h"
#include "World.h"
#include "shared/Utility/Log.h"
#include "Graphics/ResourceManager.h"
#include "Lobby.h"
#include <iostream>
#include "TextInput.h"

int main()
{
	unsigned int width{ 1280 }, height{ 720 };

	Log::Init();

	LOG_INFO("Loading Resouces...");
	ResourceManager::Instance().LoadFont("arial","Arial.ttf");
	ResourceManager::Instance().LoadTexture("player", "player.png");
	ResourceManager::Instance().LoadTexture("wall", "wall.png");
	ResourceManager::Instance().LoadTexture("floor", "floor.png");
	ResourceManager::Instance().LoadTexture("bullet", "bullet.png");
	ResourceManager::Instance().LoadTexture("skeleton", "skeleton.png");
	LOG_INFO("Resouces Loaded");

	LOG_INFO("Enter Player Name:");
	std::string playerName;
	std::cin >> playerName;

	//Create window
	sf::RenderWindow window;

	//World
	World world{ window };
	world.SetWindowSize(sf::Vector2u(width, height));

	//networking
	ServerConnection server_connection{ 8305, &world, playerName };
	server_connection.FindServer();
	server_connection.Connect();
	server_connection.GetChatBox().SetScreenSize(width, height);

	Lobby lobby{ server_connection, sf::Vector2u{width,height } };


	//Generate World
	world.Generate(&server_connection);

	window.create(sf::VideoMode(width, height), "SFML window");

	constexpr float NETWORK_TICK_RATE{ 1.0f / 32.0f };

	float m_lastNetworkTick{ 0.0 };
	float currentTime{ 0.0f };
	float lastTime{ 0.0f };
	float m_fps{ 0.0f };

	sf::Text m_fpsText = sf::Text("FPS = ", ResourceManager::Instance().GetFont("arial"), 22);
	m_fpsText.setColor(sf::Color::Blue);
	m_fpsText.setPosition(0, 0);
	sf::View uiView(sf::FloatRect(0.0f, 0.0f, width, height));

	while (window.isOpen())
	{
		float currentTime = std::chrono::duration<float>(std::chrono::steady_clock::now().time_since_epoch()).count();

		// Event processing
		sf::Event event;
		TextInput::Clear();
		while (window.pollEvent(event))
		{
			// Request for closing the window
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				width = event.size.width;
				height = event.size.height;
				uiView = sf::View(sf::FloatRect(0.0f, 0.0f, width, height));
				world.SetWindowSize(sf::Vector2u(width, height));
				server_connection.GetChatBox().SetScreenSize(width, height);
			}
			else if (event.type == sf::Event::GainedFocus)
				world.SetWindowFocused(true);
			else if (event.type == sf::Event::LostFocus)
				world.SetWindowFocused(false);
			else if (event.type == sf::Event::TextEntered && world.IsWindowFocused())
			{
				TextInput::AddKey(static_cast<char>(event.text.unicode));
			}
		}

		//wait for game to start
		if (!lobby.ShouldStart())
		{
			server_connection.PollMessages();

			lobby.Update();

			window.clear(sf::Color{ 80,64,64,255 });
			lobby.Draw(window);
			window.display();
			continue;
		}

		//update chatbox
		server_connection.GetChatBox().Update();

		//set view to camera view
		window.setView(world.GetCamera().GetView());

		//only simulate world and network 32 ticks per seconds
		if (currentTime >= m_lastNetworkTick + NETWORK_TICK_RATE)
		{
			float t = currentTime - m_lastNetworkTick;
			world.Update(t);
			server_connection.UpdateTick();
			m_lastNetworkTick = currentTime;
		}

		window.clear(sf::Color{ 80,64,64,255 });

		world.Draw(window);

		//Draw UI
		window.setView(uiView);
		m_fpsText.setString("FPS = " + std::to_string((int)m_fps) + "\nPing = " + std::to_string((int)server_connection.GetPing()));
		window.draw(m_fpsText);
		server_connection.GetChatBox().Draw(window);
		window.display();

		server_connection.PollMessages();

		m_fps = 1.0f / (currentTime - lastTime);
		lastTime = currentTime;
	}

	server_connection.Disconnect();
	return 0;
}
