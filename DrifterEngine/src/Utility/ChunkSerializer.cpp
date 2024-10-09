#include "pch.h"
#include "ChunkSerializer.h"
#include <Utility/VectorBuffer.h>
#include <Utility/CompressData.h>
#include <Snapshot/EnTTSnapshot.h>

void drft::ChunkSerializer::save(ChunkPosition position, entt::registry& registry)
{
	// Serialize registry
	VectorBuffer buffer;
	std::ostream os{ &buffer };
	cereal::BinaryOutputArchive output{ os };
	snapshot::Snapshot::save(output, registry);

	// Compress data
	auto compressed = util::compressData(buffer);

	// Get region file name

	// Open region file

	// Get lookup table

	// Get starting position in file

	// Check if enough room to write the registry

	// Write bytes to file from starting position


}

void drft::ChunkSerializer::load(ChunkPosition position, entt::registry& registry)
{

}
