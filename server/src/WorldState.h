#pragma once
#include "shared/DungeonGeneration/Dungeon.h"
#include <memory>
#include "Connection.h"
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include "shared/Collider.h"
#include "Entity.h"


class Network;
class WorldState
{
public:
    WorldState(unsigned int seed);
    ~WorldState();

	void GenerateWorld();
	void Update();

	void SetNetwork(Network& network);
	unsigned int GetSeed();
	void SpawnPlayer(Connection& connection);
	void SpawnAllEntities();
	std::shared_ptr<Entity> SpawnNewEntity(const std::string& entityName, sf::Vector2f position, sf::Vector2f velocity, unsigned int ownership, CollisionLayer layerOverride);
	void DestroyEntity(unsigned int worldID);
	void SetEntityActive(unsigned int worldID, bool active);

	void SpawnEntity(int worldID);
	void MoveEntity(int worldID, sf::Vector2f newPosition, sf::Vector2f velocity);
	void SetEntityHealth(unsigned int worldID, float health, float maxHealth);
	void DamageEntity(unsigned int worldID, float damage);

	std::unordered_map<unsigned int, std::shared_ptr<Entity>>& GetEntities() { return m_entities; }
	std::shared_mutex& GetEntityMutex() { return m_entityMapMutex; }

	bool IsColliding(unsigned int worldID, unsigned int otherWorldID) const;
private:
	Network* m_network{nullptr};
	unsigned int m_seed{ 0 };
	std::unique_ptr<Dungeon> m_dungeon;
	unsigned int entityIdCounter{ 1 };
	std::unordered_map<unsigned int, std::shared_ptr<Entity>> m_entities; // world entity id as key, a pair with postition and ownership id as the key
	std::unordered_map<unsigned int, std::shared_ptr<Entity>> m_enemies; // world entity id as key, a pair with postition and ownership id as the key
	std::unordered_map<unsigned int, std::shared_ptr<Entity>> m_players; // world entity id as key, a pair with postition and ownership id as the key

	std::shared_mutex m_entityMapMutex;
	std::unordered_set<std::shared_ptr<Collider>> m_colliders;



	sf::Vector2f findValidSpawnPos() const;
	sf::Vector2f findRandomPos() const;
	void SpawnEnemies();
	std::shared_ptr<Entity> GetClosestPlayer(const Entity& sourceEntity, float& distance) const;
	void entityWorldCollision(Entity& entity);
	void enemyCollisions();
};
