#include <pch.h>
#include "SaveRegistry.h"
#include <Snapshot/EnTTSnapshot.h>

void drft::util::saveRegistryToFile(const entt::registry& reg, std::string dirPath, std::string filename, SerializeOption option)
{
	if (reg.empty())
	{
		throw std::exception("Registry should not be empty");
	}
	using namespace snapshot;
	std::filesystem::create_directory(dirPath);
	std::string fullpath = dirPath + filename;
	{
		switch (option)
		{
			case SerializeOption::Binary:
			{
				std::ofstream ofs(fullpath + ".dat", std::ios::binary | std::ofstream::trunc);
				cereal::BinaryOutputArchive output{ ofs };
				Snapshot::save(output, reg);
			}
			break;
			case SerializeOption::JSON:
			{
				std::ofstream ofs(fullpath + ".json", std::ofstream::trunc);
				cereal::JSONOutputArchive output{ ofs };
				Snapshot::save(output, reg);
			}
			break;
		}
	}
}
