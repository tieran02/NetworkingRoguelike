#include "EntityFactory.h"
#include "Entites/Player.h"
#include "Entites/Bullet.h"
#include "Entites/Skeleton.h"
#include "Entites/MultishotPickup.h"
#include "Entites/ConeshotPickup.h"
#include "Entites/SingleshotPickup.h"
#include "Entites/FireratePickup.h"
#include "Entites/HealthPickup.h"

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
    AddEntity(0,std::make_shared<Player>());
	AddEntity(1, std::make_shared<Bullet>());
	AddEntity(2, std::make_shared<Skeleton>());
	AddEntity(3, std::make_shared<MultishotPickup>());
	AddEntity(4, std::make_shared<ConeshotPickup>());
	AddEntity(5, std::make_shared<SingleshotPickup>());
	AddEntity(6, std::make_shared<FireratePickup>());
	AddEntity(7, std::make_shared<HealthPickup>());

}

std::shared_ptr<Entity> EntityFactory::CreateEntity(unsigned int entityID, unsigned int worldID, unsigned int ownership, ServerConnection* connection, World* world) const
{
    if(m_entities.find(entityID) != m_entities.end())
    {
        return m_entities.at(entityID)->Clone(worldID,ownership, connection, world);
    }
    return nullptr;
}


void EntityFactory::AddEntity(unsigned int id, std::shared_ptr<Entity> entity)
{
    m_entities[id] = entity;
}
