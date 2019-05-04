#pragma once
#include "Message.h"


enum class TextType
{
	PLAIN,
	PLAYER_NAMES,
	CHAT
};

class TextMessage : public Message
{
public:
	TextMessage() = default;;
	TextMessage(char* buffer);
	TextMessage(const std::string& text, TextType textType);

	TextType GetTextType() const;
	std::string GetText() const;

};

