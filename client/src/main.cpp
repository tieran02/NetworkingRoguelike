#include <SFML/Graphics.hpp>
#include "DungeonGeneration/Dungeon.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <string.h>

int main()
{
	//networking
	//TCP
	sf::TcpSocket tcpSocket;

	sf::UdpSocket socket;
	char data[256]{ "broadcast message" };
	sf::IpAddress recipient = sf::IpAddress::Broadcast;
	sf::IpAddress sender;
    unsigned short server_udp_port{ 4305 };
    unsigned short server_tcp_port{ 4306 };
	std::size_t received;

	if (socket.send(data, sizeof(data), recipient, server_udp_port) != sf::Socket::Done)
	{
		std::cerr << "Failed to send data\n";
	}
	//get response from server
	if (socket.receive(data, sizeof(data), received, sender, server_udp_port) != sf::Socket::Done)
	{
		std::cout << "Failed to receive data from server\n";
	}

	sf::IpAddress serverIP;
	if (strcmp(data, "server response") == 0)
	{
		serverIP = sender;
		std::cout << "Found server: " << serverIP << ":" << server_udp_port << std::endl;

        //TCP SOCKET
        sf::Socket::Status status = tcpSocket.connect(serverIP, server_tcp_port);
        if (status != sf::Socket::Done)
        {
            std::cerr << "Failed to connect to sever TCP socket\n";
        }
        if (tcpSocket.send("TCP Connection Established", 256) != sf::Socket::Done)
        {
           std::cerr << "Failed to send tcp message to the server\n";
        }
	}


	Dungeon dungeon{ 9 };
	dungeon.Generate();

	int width{ 1280 }, height{ 720 };
	sf::RenderWindow window(sf::VideoMode(width, height), "SFML window");
	sf::View view(sf::FloatRect(0.0f, 0.0f, (float)width, (float)height));
	view.setCenter(64 / 2 * 16, 64 / 2 * 16);
	view.zoom(-2.5);

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
			if(event.type == sf::Event::Resized)
			{
				sf::View view(sf::FloatRect(0.0f, 0.0f, (float)event.size.width, (float)event.size.height));
				view.setCenter(64 / 2 * 16, 64 / 2 * 16);
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
