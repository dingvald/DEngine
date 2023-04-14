#pragma once
#include "SerializationOptions.h"

namespace drft::util
{
	void loadRegistryFromFile(entt::registry& reg, std::string dirPath, std::string filename, SerializeOption option = SerializeOption::Binary);
}
