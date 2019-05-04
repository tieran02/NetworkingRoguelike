#pragma once
#include "Networking/ServerConnection.h"

class Lobby
{
public:
	Lobby(ServerConnection& connection, sf::Vector2u screenSize);
	~Lobby();

	void Update();
	void Draw(sf::RenderWindow& window);
	bool ShouldStart() const;
private:
	ServerConnection& m_connection;
	bool m_startGame{ false };
	bool m_sentRequest{ false };
	sf::Vector2u m_screenSize;

	void setup();

	sf::Text m_mainText;
	sf::Text m_connectedPlayerTitle;
	sf::Text m_playersText;

	void AlignText(sf::Text& text, float percentX, float percentY);

};

