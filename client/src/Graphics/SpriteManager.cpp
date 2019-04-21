#include "SpriteManager.h"

bool SpriteManager::LoadTexture(const std::string& name, const std::string& path)
{
	sf::Texture texture;
	if (texture.loadFromFile("resources/" + path))
	{
		m_textures.insert(std::make_pair(name, texture));
		return true;
	}
	return false;
}

bool SpriteManager::HasTexture(const std::string& name)
{
	return m_textures.find(name) != m_textures.end();
}

std::shared_ptr<sf::Sprite> SpriteManager::CreateSprite(const std::string& textureName)
{
	if(HasTexture(textureName))
	{
		auto sprite = std::make_shared<sf::Sprite>();
		sprite->setTexture(m_textures.at(textureName));
		return sprite;
	}
	return nullptr;
}
