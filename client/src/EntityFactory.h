#pragma once
#include <unordered_map>
#include <memory>
#include "Entity.h"

class EntityFactory
{
public:
    EntityFactory();
    ~EntityFactory();

    void AddEntity(unsigned int id, std::shared_ptr<Entity> entity);
    std::shared_ptr<Entity> CreateEntity(unsigned int id);
private:
    std::unordered_map<unsigned int, std::shared_ptr<Entity>> m_entities;
};
