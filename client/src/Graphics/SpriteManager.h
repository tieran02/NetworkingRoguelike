#pragma once
#include <unordered_map>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <memory>

class SpriteManager
{
public:
	static SpriteManager& Instance()
	{
		static SpriteManager instance;
		return instance;
	}
	SpriteManager(SpriteManager const&) = delete;
	void operator=(SpriteManager const&) = delete;


	bool LoadTexture(const std::string& name, const std::string& path);
	bool HasTexture(const std::string& name);
	std::shared_ptr<sf::Sprite> CreateSprite(const std::string& textureName);
private:
	SpriteManager() {}
	std::unordered_map<std::string, sf::Texture> m_textures;
};

