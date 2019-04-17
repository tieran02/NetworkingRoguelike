#pragma once
#include "shared/DungeonGeneration/Dungeon.h"
#include <memory>
#include "Connection.h"
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

struct Entity
{
	Entity(unsigned int worldID, unsigned int entityID, sf::Vector2f pos,sf::Vector2f dir, unsigned int ownership) : WorldID(worldID), EntityID(entityID), Position(pos), Direction(dir),OwnershipID(ownership), IsActive(true){}
	Entity(){}
	unsigned int WorldID;
	unsigned int EntityID;
	sf::Vector2f Position;
	sf::Vector2f Direction;
	unsigned int OwnershipID;
	bool IsActive;
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
	void SpawnNewEntity(int entityID, sf::Vector2f position, unsigned int ownership = 0);
	void SpawnEntity(int worldID);
	void MoveEntity(int worldID, sf::Vector2f newPosition);
	std::unordered_map<unsigned int, std::shared_ptr<Entity>>& GetEntities() { return m_entities; }
	std::shared_mutex& GetEntityMutex() { return m_entityMapMutex; }
private:
	Network* m_network;
	unsigned int m_seed{ 0 };
	std::unique_ptr<Dungeon> m_dungeon;
	unsigned int entityIdCounter{ 1 };
	std::unordered_map<unsigned int, std::shared_ptr<Entity>> m_entities; // world entity id as key, a pair with postition and ownership id as the key
	std::shared_mutex m_entityMapMutex;
    unsigned int m_clientTurnID{0};

	sf::Vector2f findValidSpawnPos() const;
};
