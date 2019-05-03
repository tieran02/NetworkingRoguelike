#include "ResourceManager.h"
#include "shared/Utility/Log.h"

bool ResourceManager::LoadFont(const std::string & name, const std::string & path)
{
	sf::Font font;
	if (!font.loadFromFile("resources/" + path))
	{
		LOG_FATAL("FAILED TO LOAD FONT AT: 'resources/" + path + "'");
		return false;
	}
	m_fonts.insert(std::make_pair(name, font));
	return true;
}

const sf::Font& ResourceManager::GetFont(const std::string & name) const
{
	return m_fonts.at(name);
}

bool ResourceManager::LoadTexture(const std::string& name, const std::string& path)
{
	sf::Texture texture;
	if (texture.loadFromFile("resources/" + path))
	{
		m_textures.insert(std::make_pair(name, texture));
		return true;
	}
	LOG_FATAL("FAILED TO LOAD TEXTURE AT: 'resources/" + path + "'");
	return false;
}

bool ResourceManager::HasTexture(const std::string& name)
{
	return m_textures.find(name) != m_textures.end();
}

std::shared_ptr<sf::Sprite> ResourceManager::CreateSprite(const std::string& textureName)
{
	if(HasTexture(textureName))
	{
		auto sprite = std::make_shared<sf::Sprite>();
		sprite->setTexture(m_textures.at(textureName));
		return sprite;
	}
	return nullptr;
}
