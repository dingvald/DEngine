#include "pch.h"
#include "SimpleDecorator.h"
#include <Random/Random.h>
#include <ProcGen/GenerationContext.h>

void SimpleDecorator::decorate(SlotPositionList& slotPositions, TaggedPositions& taggedPositions, const GenerationContext& context) const
{
	drft::rng::Random random{ context.seed };
	for (auto&& position : getMyPositions(taggedPositions))
	{
		if (!meetsCondition(random)) continue;
		slotPositions.emplace_back(_slot, position);
	}
}

void SimpleDecorator::createFromJsonImpl(const rapidjson::Value& json)
{
	if (json.HasMember("slot"))
	{
		_slot = entt::hashed_string{ json["slot"].GetString() };
	}
}
