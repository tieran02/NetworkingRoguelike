#include<SFML/Network.hpp>
#include <iostream>

int main()
{
	std::cout << "Running Server\n";

    sf::UdpSocket socket;
    if(socket.bind(4300) != sf::Socket::Done)
    {
        std::cout << "failed to bind upd socket\n";
    }

    sf::IpAddress sender;
    unsigned short port;
    char data[256];
    std::size_t received;

    while(true)
    {
        if (socket.receive(data,sizeof(data),received,sender,port) != sf::Socket::Done)
        {
            std::cout << "Failed To receive packet\n";
        }

		if(strcmp(data, "broadcast message") == 0) {
			char msg[256]{ "server response" };
			if (socket.send(msg, sizeof(msg), sender, port) != sf::Socket::Done)
			{
				std::cout << "Failed To send packet\n";
			}
		}

        std::cout << data << " Received " << received << " bytes from " << sender << " on port " << port << std::endl;
    }

    return 0;
}
