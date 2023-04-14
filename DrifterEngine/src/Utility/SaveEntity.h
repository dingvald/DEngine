#pragma once
#include "SerializationOptions.h"

namespace drft::util
{
	void saveEntityToFile(entt::handle entity, std::string dirPath, std::string filename, SerializeOption option = SerializeOption::Binary);
}

