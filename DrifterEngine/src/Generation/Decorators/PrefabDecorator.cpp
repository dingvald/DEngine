#include "pch.h"
#include "PrefabDecorator.h"
#include <Random/Random.h>
#include <Generation/GenerationContext.h>
#include <Generation/GenerationRegistries.h>
#include <Spatial/Helpers.h>
#include <Spatial/ChunkPosition.h>

namespace Internal
{
	static bool fitsInChunk(const Prefab& prefab)
	{
		auto& area = prefab.getArea();
		auto chunkRect = sf::IntRect{ {0,0}, {drft::ChunkDimensions.x, drft::ChunkDimensions.y} };

		return area.size.x <= chunkRect.size.x 
			&& area.size.y <= chunkRect.size.y;
	}
}


SlotPositionList PrefabDecorator::decorateImpl(const PositionList& taggedPositions, const GenerationContext& context) const
{
	std::optional<sf::Vector3i> placementPosition = std::nullopt;

	bool shouldAlignToChunk = false;
	const Prefab& prefab = context.registries.prefabs.get(_prefabId);
	if (!Internal::fitsInChunk(prefab)) shouldAlignToChunk = true;

	for (auto&& position : taggedPositions)
	{
		if (shouldAlignToChunk)
		{
			placementPosition = drft::spatial::getChunkOrigin(position);
			break;
		}
			
		placementPosition = position;
		break;
	}

	if (!placementPosition.has_value()) return {};

	return prefab.generate(placementPosition.value(), context);
}

void PrefabDecorator::createFromJsonImpl(const rapidjson::Value& json)
{
	if (json.HasMember("prefab"))
	{
		_prefabId = entt::hashed_string{ json["prefab"].GetString() };
	}
}