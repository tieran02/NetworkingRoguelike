#include "EntityFactory.h"

EntityFactory::EntityFactory()
{
    //ctor
}

EntityFactory::~EntityFactory()
{
    //dtor
}

void EntityFactory::AddEntity(unsigned int id, std::shared_ptr<Entity> entity)
{
    m_entities[id] = entity;
}

std::shared_ptr<Entity> EntityFactory::CreateEntity(unsigned int id)
{
	return nullptr;
}
