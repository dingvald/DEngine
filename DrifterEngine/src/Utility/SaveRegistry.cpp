#include <pch.h>
#include "SaveRegistry.h"
#include <Snapshot/EnTTSnapshot.h>

void drft::util::saveRegistryToFile(const entt::registry& reg, const std::filesystem::path& filepath)
{
	using namespace snapshot;

	std::filesystem::path directory = filepath;
	std::filesystem::create_directory(directory.remove_filename());

	std::filesystem::path copypath = filepath;
	if (!copypath.has_extension())
	{
		// assume binary
		copypath.append(".dat");
	}

	if (copypath.extension() == ".json")
	{
		std::ofstream ofs(filepath, std::ofstream::trunc);
		cereal::JSONOutputArchive output{ ofs };
		Snapshot::save(output, reg);
	}
	else if (copypath.extension() == ".dat")
	{
		std::ofstream ofs(filepath, std::ios::binary | std::ofstream::trunc);
		cereal::BinaryOutputArchive output{ ofs };
		Snapshot::save(output, reg);
	}
	else
	{
		throw std::exception("Unknown file extension for saved registry");
	}
}
