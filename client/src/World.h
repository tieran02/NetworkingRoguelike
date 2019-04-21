#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "EntityFactory.h"
#include "shared/DungeonGeneration/Dungeon.h"

class Entity;
class ServerConnection;

class World
{
public:
	World();
	~World();

	void Generate(ServerConnection* connection);
	void SetSeed(unsigned int);

    void Update(float deltaTime);
	void Draw(sf::RenderWindow & window);

	std::shared_ptr<Entity> SpawnEntity(unsigned int entityID, unsigned int worldID, sf::Vector2f pos, sf::Vector2f velocity, unsigned int ownership);

	bool IsGenerated() const { return m_generated; }
	std::unordered_map<unsigned int, std::shared_ptr<Entity>>& GetEntities() { return m_entities; }
	void SetWindowFocused(bool focused);
	bool IsWindowFocused() const;

private:
	bool m_windowFocused{ true };
	bool m_generated{ false };
	unsigned int m_seed{0};
	std::unique_ptr<Dungeon> m_dungeon;
	sf::Vector2f m_spawnPos;
	std::unordered_map<unsigned int, std::shared_ptr<Entity>> m_entities;
	EntityFactory m_entityFactory;
	ServerConnection* m_serverConnection;

	std::unordered_set<std::shared_ptr<Collider>> m_colliders;

	void collisionDetection();
};

