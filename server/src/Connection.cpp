#include "Connection.h"
#include <iostream>
#include "WorldState.h"
#include <sstream>
#include "shared/Utility/Log.h"
#include "Network.h"


Connection::Connection(Network* network): m_connectionID(0), m_portTCP(0)
{
	m_network = network;
}

Connection::~Connection()
{
    LOG_INFO("Calling connection destructor!");
    m_close = true;
    m_receiveThread.detach();

}

void Connection::Connect(unsigned int id, int seed)
{
	if(m_isConnected)
	{
		std::stringstream stream;
		stream << "Client @" << m_address << ":" << m_portTCP << " is already connected!" ;
		LOG_INFO(stream.str());
		return;
	}

	m_address = m_tcpSocket->getRemoteAddress();
	m_portTCP = m_tcpSocket->getRemotePort();
	m_isConnected = true;
	m_connectionID = id;

	m_udpSocket = std::unique_ptr<sf::UdpSocket>(new sf::UdpSocket());
	m_receiveThread = std::thread(&Connection::ReceiveTCP, this);;

	std::stringstream stream;
	stream << "Client Connected @" << m_address << ":" << m_portTCP ;
	LOG_INFO(stream.str());
}

void Connection::Disconnect()
{
	m_tcpSocket->disconnect();
	m_isConnected = false;


	std::stringstream stream;
	stream << "Client Disconnected @" << m_address << ":" << m_portTCP ;
	LOG_INFO(stream.str());
}

void Connection::SetClientName(const std::string & name)
{
	m_name = name;
}

void Connection::ReceiveTCP()
{
	const size_t maxMessageSize = Config::MAX_PACKET_SIZE;
	char buffer[maxMessageSize];
	size_t received;

	while (!m_close)
	{
        if(m_tcpSocket != nullptr && m_tcpSocket->getLocalPort() == 0)
        {
            continue;
        }

		std::memset(buffer, 0, maxMessageSize);

        //error
		auto data = m_tcpSocket->receive(buffer, maxMessageSize, received);
		if (data != sf::Socket::Done)
		{
			if (data == sf::Socket::Disconnected)
			{
			    std::stringstream stream;
                stream << "Disconnect from client:" << m_connectionID ;
                LOG_WARN(stream.str());
				return;
			}
			else if(data == sf::Socket::NotReady)
			{
                LOG_ERROR("TCP socket wasn't ready");
			}
			LOG_ERROR("Failed To receive tcp packet");
		}
		else
		{
			std::stringstream stream;
			stream << "Recieved TCP message from client:" << m_connectionID ;
			LOG_TRACE(stream.str());
		}

		if(received <= 0)
		{
            LOG_FATAL("MESSAGE IS EMPTY!");
            continue;
		}

        if(received < sizeof(Header) || received > maxMessageSize)
            continue;

		Message message{ buffer };

		if (message.GetHeader().type == MessageType::BATCH)
		{
			BatchMessage* batch = static_cast<BatchMessage*>(&message);
			auto count = batch->GetCount();

			for (auto i = 0; i < count; i++)
			{
				ServerMessage serverMessage(batch->GetMessageAt(i));
				serverMessage.protocol = Protocol::TCP;
				serverMessage.senderAddress = m_address;
				serverMessage.senderPort = m_portTCP;
				m_network->GetMessageQueue().enqueue(serverMessage);
			}
		}
		else
		{
			ServerMessage serverMessage(message);
			serverMessage.protocol = Protocol::TCP;
			serverMessage.senderAddress = m_address;
			serverMessage.senderPort = m_portTCP;

			if (serverMessage.message.GetHeader().type == MessageType::CLIENT_SETUP)
			{
				m_isSetup = true;
				m_cv.notify_all();
				continue;
			}
			m_network->GetMessageQueue().enqueue(serverMessage);
		}
	}
}

void Connection::SendTCP(const Message& msg) const
{
	const auto buffer = msg.GetBuffer();
	if (m_tcpSocket->send(buffer.data(), buffer.size()) != sf::Socket::Done)
	{
		std::stringstream stream;
		stream << "Failed To send message over TCP to client: " << m_connectionID;
		LOG_ERROR(stream.str());
	}
	std::this_thread::sleep_for(std::chrono::microseconds(500));
}

void Connection::SendUDP(const Message& msg) const
{
	auto buffer = msg.GetBuffer();
	if (m_udpSocket->send(buffer.data(), buffer.size(), m_address, m_portUDP) != sf::Socket::Done)
	{
		std::stringstream stream;
		stream << "Failed To send message over UDP to client: " << m_connectionID;
		LOG_ERROR(stream.str());
	}
}
