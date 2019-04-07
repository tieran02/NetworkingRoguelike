#pragma once
#include <unordered_map>
#include <memory>
#include "Entity.h"

class EntityFactory
{
public:
    EntityFactory();
    ~EntityFactory();

    void Setup();

    std::shared_ptr<Entity> CreateEntity(unsigned int entityID, unsigned int worldID, unsigned int ownership, ServerConnection* connection) const;
private:
    std::unordered_map<unsigned int, std::shared_ptr<Entity>> m_entities;

    void AddEntity(unsigned int id, std::shared_ptr<Entity> entity);
};
