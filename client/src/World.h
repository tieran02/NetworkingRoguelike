#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "EntityFactory.h"
#include "shared/DungeonGeneration/Dungeon.h"
#include "Camera.h"

class Entity;
class ServerConnection;

class World
{
	friend ServerConnection;
public:
	World();
	~World();

	void Generate(ServerConnection* connection);
	void SetSeed(unsigned int);

    void Update(float deltaTime);
	void Draw(sf::RenderWindow& window);

	std::shared_ptr<Entity> SpawnEntity(unsigned int entityID, unsigned int worldID, sf::Vector2f pos, sf::Vector2f velocity, unsigned int ownership);
	void ShootBullet(sf::Vector2f startPos, sf::Vector2f velocity);

	void RequestDestroyEntity(unsigned int worldID);

	bool IsGenerated() const { return m_generated; }
	std::unordered_map<unsigned int, std::shared_ptr<Entity>>& GetEntities() { return m_entities; }
	void SetWindowFocused(bool focused);
	bool IsWindowFocused() const;

	void SetWindowSize(sf::Vector2u size);
	sf::Vector2u GetWindowSize() const;

	Camera& GetCamera();
private:
	Camera m_camera;

	bool m_windowFocused{ true };
	bool m_generated{ false };
	unsigned int m_seed{0};
	std::unique_ptr<Dungeon> m_dungeon;
	sf::Vector2f m_spawnPos;
	std::unordered_map<unsigned int, std::shared_ptr<Entity>> m_entities;
	std::vector<std::shared_ptr<Entity>> m_entitiesToDestroy;
	EntityFactory m_entityFactory;
	ServerConnection* m_serverConnection{};
	sf::Vector2u m_windowSize;

	std::unordered_set<std::shared_ptr<Collider>> m_colliders;
	std::shared_ptr<sf::Sprite> m_wallSprite;
	std::shared_ptr<sf::Sprite> m_floorSprite;

	void collisionDetection();
	bool m_debug{ false };

	void removeEntity(unsigned int worldID);
};

