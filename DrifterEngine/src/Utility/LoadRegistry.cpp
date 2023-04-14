#include "pch.h"
#include "LoadRegistry.h"
#include <Snapshot/EnTTSnapshot.h>

void drft::util::loadRegistryFromFile(entt::registry& reg, std::string dirPath, std::string filename, SerializeOption option)
{
	if (!reg.empty())
	{
		throw std::exception("Registry should be empty");
	}
	using namespace snapshot;

	std::string fullpath = dirPath + filename;
	{
		switch (option)
		{
			case SerializeOption::Binary:
			{
				std::ifstream ifs(fullpath + ".dat", std::ios::binary);
				cereal::BinaryInputArchive input{ ifs };
				SnapshotLoader::load(input, reg);
			}
			break;
			case SerializeOption::JSON:
			{
				std::ifstream ifs(fullpath + ".json");
				cereal::JSONInputArchive input{ ifs };
				SnapshotLoader::load(input, reg);
			}
			break;
		}
	}
}
