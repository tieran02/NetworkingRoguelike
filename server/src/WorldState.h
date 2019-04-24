#pragma once
#include "shared/DungeonGeneration/Dungeon.h"
#include <memory>
#include "Connection.h"
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include "shared/Collider.h"
#include "shared/EntityBaseData.h"
#include "shared/EntityDataManager.h"

class Entity
{
public:
	Entity(const std::string& entityName, unsigned int worldID, sf::Vector2f pos, sf::Vector2f velocity, unsigned int ownership, CollisionLayer layerOveride) :
		m_worldID(worldID),
		m_position(pos),
		m_velocity(velocity),
		m_ownershipID(ownership),
		m_active(true)
	{
		//TODO load all entities
		m_baseData = EntityDataManager::Instance().GetEntityData(entityName);
		m_collider = std::make_shared<Collider>(m_position, sf::Vector2f{ (float)m_baseData.SpriteWidth,(float)m_baseData.SpriteHeight }, this, (CollisionLayer)m_baseData.Layer);
	}
	Entity(){}

	const EntityBaseData& BaseData() const { return m_baseData; }
	const unsigned int& WorldID() const { return m_worldID; }
	const sf::Vector2f& Position() const { return m_position; }
	const sf::Vector2f& Velocity() const { return m_velocity; }
	const unsigned int& OwnershipID() const { return m_ownershipID; }
	const bool& IsActive() const { return m_active; }
	std::shared_ptr<Collider> GetCollider() { return m_collider; }

	void SetPosition(sf::Vector2f pos) { m_position = pos; m_collider->SetPosition(pos); }
	void SetVelocity(sf::Vector2f velocity) { m_velocity = velocity; }
	void SetActive(bool active) { m_active = active; m_collider->SetActive(active); }
	void SetHealth(float amount) { m_baseData.Health = amount; }
	void SetMaxHealth(float amount) { m_baseData.MaxHealth = amount; }

private:
	EntityBaseData m_baseData;
	unsigned int m_worldID;
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	unsigned int m_ownershipID;
	bool m_active;
	std::shared_ptr<Collider> m_collider;

};

class Network;
class WorldState
{
public:
    WorldState(unsigned int seed);
    ~WorldState();

	void GenerateWorld();

	void SetNetwork(Network& network);
	unsigned int GetSeed();
	void SpawnPlayer(Connection& connection);
	void SpawnAllEntities();
	void SpawnNewEntity(const std::string& entityName, sf::Vector2f position, sf::Vector2f velocity, unsigned int ownership, CollisionLayer layerOverride);
	void DestroyEntity(unsigned int worldID);
	void SetEntityActive(unsigned int worldID, bool active);

	void SpawnEntity(int worldID);
	void MoveEntity(int worldID, sf::Vector2f newPosition, sf::Vector2f velocity);
	void SetEntityHealth(unsigned int worldID, float health, float maxHealth);
	std::unordered_map<unsigned int, std::shared_ptr<Entity>>& GetEntities() { return m_entities; }
	std::shared_mutex& GetEntityMutex() { return m_entityMapMutex; }
private:
	Network* m_network{nullptr};
	unsigned int m_seed{ 0 };
	std::unique_ptr<Dungeon> m_dungeon;
	unsigned int entityIdCounter{ 1 };
	std::unordered_map<unsigned int, std::shared_ptr<Entity>> m_entities; // world entity id as key, a pair with postition and ownership id as the key
	std::shared_mutex m_entityMapMutex;
	std::unordered_set<std::shared_ptr<Collider>> m_colliders;



	sf::Vector2f findValidSpawnPos() const;

	sf::Vector2f findRandomPos() const;
	void SpawnEnemies();

};
