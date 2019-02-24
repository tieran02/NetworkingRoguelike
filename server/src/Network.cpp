#include "Network.h"
#include "Connection.h"
#include <iostream>
#include <SFML/Network.hpp>
#include <thread>
#include "Message.h"

Network::Network(unsigned short port) : UDP_PORT(port), TCP_PORT(port+1)
{
}

Network::~Network()
{
}

void Network::Start()
{
	m_running = true;
	std::cout << "Running Server\n";

	//BIND UPD
	if (m_udpSocket.bind(UDP_PORT) != sf::Socket::Done)
	{
		std::cerr << "failed to bind upd socket\n";
	}

	std::thread tcpAcceptThread(&Network::acceptTCP,this);
	std::thread updRecieve(&Network::retrieveUDP, this);
	std::thread tcpRecieve(&Network::retrieveTCP, this);

	while (m_running)
	{
		while (!m_retrievedMessages.empty())
		{
			//Get Data
			Message msg = m_retrievedMessages.front();
			m_retrievedMessages.pop();

			switch (msg.protocol)
			{
			case Protocol::UPD:
				std::cout << "UDP::Received " << msg.header.size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;
				break;
			case Protocol::TCP:
				std::cout << "TCP::Received " << msg.header.size << " bytes from " << msg.senderAddress << " on port " << msg.senderPort << std::endl;
				break;
			default: 
				break;
			}


			if (strcmp(msg.data, "broadcast message") == 0) //UDP broadcast (used to find a server)
			{
				std::string responseMsg{ "server response" };
				Message response{ responseMsg };
				sf::Packet packet;
				packet << response;

				if (m_udpSocket.send(packet, msg.senderAddress, msg.senderPort) != sf::Socket::Done)
				{
					std::cerr << "Failed To send packet\n";
				}
			}
		}

	}
	m_running = false;

	tcpAcceptThread.join();
	updRecieve.join();
	tcpRecieve.join();

}

void Network::acceptTCP()
{
	sf::TcpListener listener;
	if (listener.listen(TCP_PORT) != sf::Socket::Done)
	{
		std::cerr << "failed to listen on tcp\n";
	}

	while (m_running)
	{
		std::unique_ptr<sf::TcpSocket> client{ new sf::TcpSocket };

		if (listener.accept(*client) != sf::Socket::Done)

		{
			std::cerr << "failed to accept tcp client\n";
		}
		else
		{
			std::unique_ptr<Connection> connection{ new Connection };
			connection->Connect(client);

			m_connections.push_back(std::move(connection));

		}
	};
}

void Network::retrieveUDP()
{
	sf::IpAddress sender;
	unsigned short port;
	sf::Packet packet;

	while (m_running)
	{
		if (m_udpSocket.receive(packet, sender, port) != sf::Socket::Done)
		{
			std::cerr << "Failed To receive udp packet\n";
		}
		Message message;
		packet >> message;
		message.protocol = Protocol::UPD;
		message.senderAddress = sender;
		message.senderPort = port;
		m_retrievedMessages.push(message);
	}
}

void Network::retrieveTCP()
{
	sf::Packet packet;

	while (m_running) 
	{
		for (int i = 0; i < m_connections.size(); ++i)
		{
			if(m_connections[i] == nullptr || !m_connections[i]->IsConnected())
				continue;

			auto send = m_connections[i]->GetTcpSocket().receive(packet);
			if (send != sf::Socket::Done)
			{
				if(send == sf::Socket::Disconnected)
				{
					m_connections[i]->Disconnect();
					continue;;
				}
				std::cerr << "Failed To receive tcp packet\n";
			}
			Message message;
			packet >> message;
			message.protocol = Protocol::TCP;
			message.senderAddress = m_connections[i]->GetAddress();
			message.senderPort = m_connections[i]->GetPort();
			m_retrievedMessages.push(message);
		}
	}
}
