#include <pch.h>
#include "SaveRegistry.h"
#include <Snapshot/EnTTSnapshot.h>

void drft::util::saveRegistryToFile(const entt::registry& reg, const char* filepath)
{
	if (reg.empty())
	{
		throw std::exception("Registry should not be empty");
	}
	using namespace snapshot;
	std::ofstream ofs(filepath, std::ios::binary | std::ofstream::trunc);
	{
		cereal::BinaryOutputArchive output{ ofs };
		Snapshot::save(output, reg);
	}
}
