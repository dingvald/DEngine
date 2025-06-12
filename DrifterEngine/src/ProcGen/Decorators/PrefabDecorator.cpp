#include "pch.h"
#include "PrefabDecorator.h"
#include <Random/Random.h>
#include <ProcGen/GenerationContext.h>
#include <ProcGen/GenerationRegistries.h>
#include <Spatial/Helpers.h>
#include <Spatial/ChunkPosition.h>

namespace Internal
{
	static bool fitsInChunk(const Prefab& prefab)
	{
		auto area = prefab.getVolume().flatten();
		auto chunkRect = sf::IntRect{ {0,0}, {drft::ChunkDimensions.x, drft::ChunkDimensions.y} };

		return area.size.x <= chunkRect.size.x 
			&& area.size.y <= chunkRect.size.y;
	}
}


void PrefabDecorator::decorate(SlotPositionList& inOutSlotPositions, TaggedPositions& inOutTaggedPositions, const GenerationContext& context) const
{
	drft::rng::Random random{ generateUniqueSeed<PrefabDecorator>(context.seed) };

	std::optional<sf::Vector3i> placementPosition = std::nullopt;

	bool shouldAlignToChunk = false;
	auto& prefab = context.registries.prefabs.get(_prefabId);
	if (!Internal::fitsInChunk(prefab)) shouldAlignToChunk = true;

	for (auto&& position : getMyPositions(inOutTaggedPositions))
	{
		if (!meetsCondition(random)) continue;
		if (shouldAlignToChunk)
		{
			placementPosition = drft::spatial::getChunkOrigin(position);
			break;
		}
			
		placementPosition = position;
		break;
	}

	if (!placementPosition.has_value()) return;

	auto result = prefab.generate(placementPosition.value(), context);
	inOutSlotPositions.append_range(std::move(result));
}

void PrefabDecorator::createFromJsonImpl(const rapidjson::Value& json)
{
	if (json.HasMember("prefab"))
	{
		_prefabId = entt::hashed_string{ json["prefab"].GetString() };
	}
}