#pragma once
#include <SFML/Graphics.hpp>

class Tileset
{
public:
	Tileset(std::string path, int rows, int columns);
	~Tileset();

	sf::Sprite GetSprite(int index) const;
private:
	sf::Texture m_texture;
	int m_rows{0}, m_columns{0};
};

