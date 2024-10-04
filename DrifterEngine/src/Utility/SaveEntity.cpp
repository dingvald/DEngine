#include "pch.h"
#include "SaveEntity.h"
#include "CopyEntity.h"
#include "SaveRegistry.h"

void drft::util::saveEntityToFile(entt::handle entity, const std::filesystem::path& path)
{
	entt::registry tempReg;
	auto tempEnt = tempReg.create();
	util::copyEntity(tempEnt, entity.entity(), tempReg, *entity.registry());

	util::saveRegistryToFile(tempReg, path);
}
