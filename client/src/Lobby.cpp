#include "Lobby.h"
#include "Graphics/ResourceManager.h"
#include "TextInput.h"

Lobby::Lobby(ServerConnection& connection, sf::Vector2u screenSize) : m_connection(connection), m_screenSize(screenSize)
{
	setup();
}

Lobby::~Lobby()
{
}

void Lobby::Update()
{
	if (m_connection.InProgress())
	{
		m_startGame = true;
	}

	//chatbox input
	m_connection.GetChatBox().Update();

	std::string playerNames = "";

	for (size_t i = 0; i < m_connection.GetPlayerNames().size(); i++)
	{
		if (i == 0) //add first name
		{
			playerNames = m_connection.GetPlayerNames()[i];
		}
		else
		{
			playerNames.append("\n" + m_connection.GetPlayerNames()[i]);
		}
	}
	m_playersText.setString(playerNames);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !m_sentRequest && !m_connection.GetChatBox().IsActive())
	{
		Message gameStart{ MessageType::GAME_START,nullptr,0,(uint16_t)m_connection.GetClientID() };
		m_connection.SendTcpMessage(gameStart);
		m_sentRequest = true;
	}

}

void Lobby::Draw(sf::RenderWindow & window)
{
	window.draw(m_mainText);
	window.draw(m_connectedPlayerTitle);
	window.draw(m_playersText);
	m_connection.GetChatBox().Draw(window);

}

bool Lobby::ShouldStart() const
{
	return m_startGame;
}

void Lobby::setup()
{

	m_mainText = sf::Text("Lobby", ResourceManager::Instance().GetFont("arial"), 48);
	AlignText(m_mainText, 2.0f, 16.0f);

	m_connectedPlayerTitle = sf::Text("Connected Players:", ResourceManager::Instance().GetFont("arial"), 30);
	AlignText(m_connectedPlayerTitle, 16.0f, 4.0f);

	m_playersText = sf::Text("Player 1", ResourceManager::Instance().GetFont("arial"), 28);
	AlignText(m_playersText, 16.0f, 3.25f);
}

void Lobby::AlignText(sf::Text & text, float percentX, float percentY)
{
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / percentX,
		textRect.top + textRect.height / percentY);
	text.setPosition(sf::Vector2f((float)m_screenSize.x / percentX, (float)m_screenSize.y / percentY));
}
