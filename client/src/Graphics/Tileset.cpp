#include "Tileset.h"
#include <iostream>

Tileset::Tileset(std::string path, int rows, int columns)
{
	if(!m_texture.loadFromFile(path))
	{
		std::cerr << "Failed to load tileset from " << path << std::endl;
		return;
	}
}

Tileset::~Tileset()
{
}

sf::Sprite Tileset::GetSprite(int index) const
{
	int row = index / m_texture.getSize().x;
	int column = index % m_texture.getSize().x;
	int tileSize = m_texture.getSize().x / row;
	sf::IntRect rect(row * tileSize, column * tileSize, tileSize, tileSize);
	return sf::Sprite(m_texture, rect);
}
