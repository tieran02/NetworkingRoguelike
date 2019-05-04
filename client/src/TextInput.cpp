#include "TextInput.h"

std::queue<char> TextInput::m_keysPressed = std::queue<char>();

void TextInput::AddKey(char key)
{
	m_keysPressed.push(key);
}

char TextInput::GetKey()
{
	char key = m_keysPressed.front();
	m_keysPressed.pop();
	return key;
}

bool TextInput::IsEmpty()
{
	return m_keysPressed.empty();
}

void TextInput::Clear()
{
	m_keysPressed = std::queue<char>();
}
