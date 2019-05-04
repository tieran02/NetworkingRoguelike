#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class ServerConnection;
class ChatBox
{
public:
	ChatBox(float x, float y, ServerConnection* connection);

	void Update();
	void Draw(sf::RenderWindow& window);
	bool IsActive() const { return m_inputMode; }
	void AddMessage(const std::string& message);
	void SetConnection(ServerConnection* connection) { m_connection = connection; }
	void SetScreenSize(unsigned int width, unsigned int height) { m_screenWidth = width, m_screenHeight = height; calculatePos(); }
private:
	float m_percentX;
	float m_percentY;
	unsigned int m_screenWidth;
	unsigned int m_screenHeight;
	ServerConnection* m_connection;

	bool m_inputMode{ false };
	std::string m_inputString;
	std::vector<std::string> m_messages;

	sf::Text m_chatInputText;

	void calculatePos();
};

