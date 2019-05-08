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
	//Size of message includng header
	const static size_t SIZE;

	TextMessage() = default;;
	TextMessage(char* buffer);
	TextMessage(const std::string& text, TextType textType, unsigned int senderID);

	TextType GetTextType() const;
	std::string GetText() const;

};

