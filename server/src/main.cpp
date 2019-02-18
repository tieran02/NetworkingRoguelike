#include<SFML/Network.hpp>
#include <iostream>
#include <string.h>
#include "Connection.h"

void Connect()
{

}

int main()
{
    std::cout << "Running Server\n";
    const unsigned short PORT{4305};
    const unsigned short TCP_PORT{4310};

    sf::UdpSocket socket;
    if(socket.bind(PORT) != sf::Socket::Done)
    {
        std::cerr << "failed to bind upd socket\n";
    }

    sf::IpAddress sender;
    unsigned short port;
    char data[256];
    std::size_t received;

    //TCP
    sf::TcpListener listener;
    sf::TcpSocket client;
    bool clientConnected{false};

    if(listener.listen(TCP_PORT) != sf::Socket::Done)
    {
        std::cerr << "failed to listen on tcp\n";
    }
    while(true)
    {

        if (socket.receive(data,sizeof(data),received,sender,port) != sf::Socket::Done)
        {
            std::cerr << "Failed To receive udp packet\n";
        }


        //UDP
        if(strcmp(data, "broadcast message") == 0) //UDP broadcast (used to find a server)
        {
            char msg[256] { "server response" };
            if (socket.send(msg, sizeof(msg), sender, port) != sf::Socket::Done)
            {
                std::cerr << "Failed To send packet\n";
            }
        }
        else if(strcmp(data, "connection message") == 0) //Create TCP connection with the client
        {
            if (listener.accept(client) != sf::Socket::Done)

            {
                std::cerr << "failed to accept tcp client\n";
            }
            else
                clientConnected = true;
        }

        std::cout << data << " Received " << received << " bytes from " << sender << " on port " << port << std::endl;

        //TCP
        if(clientConnected)
        {
            if (client.receive(data, 256, received) != sf::Socket::Done)
            {
                std::cerr << "Failed To receive tcp packet\n";
            }
            else
                std::cout << data << "  TCP::Received " << received << " bytes" << std::endl;
        }
    }

    return 0;
}
