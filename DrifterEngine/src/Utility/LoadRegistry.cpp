#include "pch.h"
#include "LoadRegistry.h"
#include <Snapshot/EnTTSnapshot.h>

void drft::util::loadRegistryFromFile(entt::registry& reg, const char* filepath)
{
	if (!reg.empty())
	{
		throw std::exception("Registry should be empty");
	}
	using namespace snapshot;
	std::ifstream ifs(filepath, std::ios::binary);

	cereal::BinaryInputArchive input{ ifs };
	SnapshotLoader::load(input, reg);
}
