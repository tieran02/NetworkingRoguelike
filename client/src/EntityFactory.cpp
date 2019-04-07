#include "EntityFactory.h"
#include "Player.h"

EntityFactory::EntityFactory()
{
    //ctor
}

EntityFactory::~EntityFactory()
{
    //dtor
}

void EntityFactory::Setup()
{
    std::shared_ptr<Player> playerEntity = std::make_shared<Player>();
    AddEntity(0,std::make_shared<Player>());
}

std::shared_ptr<Entity> EntityFactory::CreateEntity(unsigned int entityID, unsigned int worldID, unsigned int ownership, ServerConnection* connection) const
{
    if(m_entities.find(entityID) != m_entities.end())
    {
        return m_entities.at(entityID)->Clone(worldID,ownership, connection);
    }
    return nullptr;
}


void EntityFactory::AddEntity(unsigned int id, std::shared_ptr<Entity> entity)
{
    m_entities[id] = entity;
}
