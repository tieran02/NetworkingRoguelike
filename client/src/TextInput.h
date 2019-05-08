#pragma once
#include <queue>

class TextInput
{
public:
	static void AddKey(char key);
	static char GetKey();
	static bool IsEmpty();
	static void Clear();
private:
	static std::queue<char> m_keysPressed;
};

