#include "pch.h"
#include "LoadEntity.h"
#include "LoadRegistry.h"
#include "CopyEntity.h"

void drft::util::loadEntityFromFile(entt::handle entity, const std::filesystem::path& path)
{
	entt::registry temp;
	util::loadRegistryFromFile(temp, path);
	if (temp.size() != 1)
	{
		throw std::exception("The registry should have exactly one entity. Use loadRegistryFromFile() instead.");
	}
	
	temp.each([&temp, &entity](auto ent)
		{
			util::copyEntity(entity.entity(), ent, *(entity.registry()), temp);
		});
}
