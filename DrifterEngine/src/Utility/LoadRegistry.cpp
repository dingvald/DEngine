#include "pch.h"
#include "LoadRegistry.h"
#include <Snapshot/EnTTSnapshot.h>

void drft::util::loadRegistryFromFile(entt::registry& reg, const std::filesystem::path& path)
{
	using namespace snapshot;

	if (!reg.empty())
	{
		throw std::exception("Registry should be empty");
	}

	std::filesystem::path copypath = path;
	if (!copypath.has_extension())
	{
		// assume binary
		copypath.append(".dat");
	}

	if (copypath.extension() == ".json")
	{
		std::ifstream ifs(copypath);
		cereal::JSONInputArchive input{ ifs };
		SnapshotLoader::load(input, reg);
	}
	else if (copypath.extension() == ".dat")
	{
		std::ifstream ifs(copypath, std::ios::binary);
		cereal::BinaryInputArchive input{ ifs };
		SnapshotLoader::load(input, reg);
	}
	else
	{
		throw std::exception("Unknown file extension for saved registry");
	}
}
