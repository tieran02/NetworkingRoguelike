#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <memory>

class ResourceManager
{
public:
	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}
	ResourceManager(ResourceManager const&) = delete;
	void operator=(ResourceManager const&) = delete;

	bool LoadFont(const std::string& name, const std::string& path);
	const sf::Font& GetFont(const std::string& name) const;

	bool LoadTexture(const std::string& name, const std::string& path);
	bool HasTexture(const std::string& name);
	std::shared_ptr<sf::Sprite> CreateSprite(const std::string& textureName);
private:
	ResourceManager() {}
	std::unordered_map<std::string, sf::Texture> m_textures;
	std::unordered_map<std::string, sf::Font> m_fonts;
};

