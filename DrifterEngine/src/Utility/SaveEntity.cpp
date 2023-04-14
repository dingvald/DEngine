#include "pch.h"
#include "SaveEntity.h"
#include "CopyEntity.h"
#include "SaveRegistry.h"

void drft::util::saveEntityToFile(entt::handle entity, std::string dirPath, std::string filename, SerializeOption option)
{
	entt::registry tempReg;
	auto tempEnt = tempReg.create();
	util::copyEntity(tempEnt, entity.entity(), tempReg, *entity.registry());

	util::saveRegistryToFile(tempReg, dirPath, filename, option);
	entity.destroy();
}
