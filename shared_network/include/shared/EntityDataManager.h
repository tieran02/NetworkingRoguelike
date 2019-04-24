#pragma once
#include <unordered_map>
#include "EntityBaseData.h"

class EntityDataManager
{
public:
	static EntityDataManager& Instance()
	{
		static EntityDataManager instance;
		return instance;
	}
	EntityDataManager(EntityDataManager const&) = delete;
	void operator=(EntityDataManager const&) = delete;

	const EntityBaseData& GetEntityData(unsigned int entityID);
	const EntityBaseData& GetEntityData(const std::string& entityName);

private:
	EntityDataManager()
	{
		load();
	}
	std::unordered_map<std::string, EntityBaseData> m_entityMap;
	void load();
};

