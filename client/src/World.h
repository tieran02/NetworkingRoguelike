#pragma once
#include "DungeonGeneration/Dungeon.h"
#include <memory>
#include <SFML/Graphics.hpp>

class Entity;
class World
{
public:
	World();
	~World();

	void Generate();
	void SetSeed(unsigned int);
    void SpawnPlayer();

	void Draw(sf::RenderWindow & window);

	void SpawnEntity(int id);
private:
	unsigned int m_seed{0};
	std::unique_ptr<Dungeon> m_dungeon;
	sf::Vector2f m_spawnPos;
	std::vector<Entity*> m_entities;

	void FindSpawnPoint();

};

