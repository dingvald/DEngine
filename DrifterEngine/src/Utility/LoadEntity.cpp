#include "pch.h"
#include "LoadEntity.h"
#include "LoadRegistry.h"
#include "CopyEntity.h"

entt::handle drft::util::loadEntityFromFile(entt::handle entity, std::string dirPath, std::string filename, SerializeOption option)
{
	entt::registry temp;
	util::loadRegistryFromFile(temp, dirPath, filename, option);
	if (temp.size() != 1)
	{
		throw std::exception("The registry should have exactly one entity. Use loadRegistryFromFile() instead.");
	}
	
	temp.each([&temp, &entity](auto ent)
		{
			util::copyEntity(entity.entity(), ent, *(entity.registry()), temp);
		});
	
	return entity;
}
