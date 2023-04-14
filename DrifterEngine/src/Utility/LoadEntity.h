#pragma once
#include "SerializationOptions.h"

namespace drft::util
{
	entt::handle loadEntityFromFile(entt::handle entity, std::string dirPath, std::string filename, SerializeOption option = SerializeOption::Binary);
}

