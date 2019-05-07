#include "ChatBox.h"
#include "TextInput.h"
#include "Graphics/ResourceManager.h"
#include "Networking/ServerConnection.h"

ChatBox::ChatBox(float x, float y, ServerConnection* connection)
	: m_percentX(x),
	m_percentY(y),
	m_connection(connection)
{
	m_chatInputText = sf::Text("Chat(ESC TO CANCEL): ", ResourceManager::Instance().GetFont("arial"), 30);

	calculatePos();
}

void ChatBox::Update()
{
	//chatbox input
	if (m_inputMode)
	{
		while (!TextInput::IsEmpty())
		{
			char key = TextInput::GetKey();
			if (key == '\b' && !m_inputString.empty())
			{
				m_inputString.erase(m_inputString.size() - 1, 1);
			}
			else
				m_inputString += key;
		}

		std::string previousMessages;
		for (const auto& msg : m_messages)
		{
			previousMessages += msg + "\n";
		}

		m_chatInputText.setString(previousMessages + "Chat(ESC TO CANCEL): " + m_inputString);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) && !m_inputMode)
	{
		m_inputMode = true;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && m_inputMode)
	{
		m_chatInputText.setString("Chat(ESC TO CANCEL): ");
		m_inputMode = false;
		m_inputString.clear();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && m_inputMode)
	{
		m_inputMode = false;
		m_chatInputText.setString("Chat(ESC TO CANCEL): ");
		//remove last character
		if (m_inputString.empty())
		{
			return;
		}
		else
		{
			m_inputString.pop_back();
		}

		std::string msg = m_connection->GetPlayerName() + ": " + m_inputString;
		//Sent text to server
		m_messages.push_back(msg);
		//sent to rest
		m_connection->SendChatMessage(msg);

		m_inputString.clear();
		calculatePos();
	}
}

void ChatBox::Draw(sf::RenderWindow & window)
{
	if (m_inputMode)
	{
		window.draw(m_chatInputText);
	}
}

void ChatBox::AddMessage(const std::string & message)
{
	m_messages.push_back(message);
}

void ChatBox::calculatePos()
{
	float yOffset = (m_messages.size() * 34.0f);

	sf::FloatRect textRect = m_chatInputText.getLocalBounds();
	m_chatInputText.setOrigin(textRect.left + textRect.width / m_percentX, (textRect.top + textRect.height / m_percentY) + yOffset);
	m_chatInputText.setPosition(sf::Vector2f((float)m_screenWidth / m_percentX, (float)m_screenHeight / m_percentY));
}
