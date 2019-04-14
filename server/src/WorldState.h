#pragma once
#include "shared/DungeonGeneration/Dungeon.h"
#include <memory>
#include "Connection.h"
#include <unordered_map>

struct Entity
{
	Entity(unsigned int worldID, unsigned int entityID, sf::Vector2f pos, unsigned int ownership) : WorldID(worldID), EntityID(entityID), Position(pos),OwnershipID(ownership){}
	Entity(){}
	unsigned int WorldID;
	unsigned int EntityID;
	sf::Vector2f Position;
	unsigned int OwnershipID;
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
private:
	Network* m_network;
	unsigned int m_seed{ 0 };
	std::unique_ptr<Dungeon> m_dungeon;
	unsigned int entityIdCounter{ 1 };
	std::unordered_map<unsigned int, std::shared_ptr<Entity>> m_entities; // world entity id as key, a pair with postition and ownership id as the key
    unsigned int m_clientTurnID{0};

	sf::Vector2f findValidSpawnPos() const;
};
