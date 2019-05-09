#include "EntityDataManager.h"
#include <json.hpp>
#include <fstream>
#include "Utility/Log.h"

using json = nlohmann::json;

const EntityBaseData& EntityDataManager::GetEntityData(unsigned entityID)
{
	for (const auto& entity : m_entityMap)
	{
		if(entity.second.EntityID == entityID)
			return entity.second;
	}
}

const EntityBaseData& EntityDataManager::GetEntityData(const std::string& entityName)
{
	return m_entityMap.at(entityName);
}

void EntityDataManager::load()
{
	std::ifstream jsonFile("./resources/entities.json");
	if(!jsonFile)
	{
		LOG_FATAL("FAILED TO LOAD ENTITIES FILE!");
		return;
	}
	json j = j.parse(jsonFile);

	for (auto& element : j.at("entities"))
	{
		EntityBaseData baseData;
		baseData.EntityID = element.at("EntityID").get<unsigned int>();
		baseData.EntityName = element.at("EntityName").get<std::string>();
		baseData.EntitySpriteName = element.at("EntitySpriteName").get<std::string>();
		baseData.SpriteWidth = element.at("SpriteWidth").get<unsigned int>();
		baseData.SpriteHeight = element.at("SpriteHeight").get<unsigned int>();
		baseData.Health = element.at("Health").get<float>();
		baseData.MaxHealth = element.at("MaxHealth").get<float>();
		baseData.Layer = element.at("Layer").get<CollisionLayer>();
		baseData.MovementSpeed = element.at("MoveSpeed").get<float>();
		baseData.ServerCollisions = element.at("ServerCollisions").get<bool>();

		m_entityMap.insert(std::make_pair(baseData.EntityName, baseData));
	}
}
