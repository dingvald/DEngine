#pragma once
#include "SerializationOptions.h"

namespace drft::util
{
	void saveRegistryToFile(const entt::registry& reg, std::string dirPath, std::string filename, SerializeOption option = SerializeOption::Binary);
}