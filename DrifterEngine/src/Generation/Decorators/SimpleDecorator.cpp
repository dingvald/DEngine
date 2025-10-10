#include "pch.h"
#include "SimpleDecorator.h"
#include <Generation/GenerationContext.h>

SlotPositionList SimpleDecorator::decorateImpl(const PositionList& taggedPositions, const GenerationContext&) const
{
	SlotPositionList result;
	for (auto&& position : taggedPositions)
	{
		result.emplace_back(_slot, position);
	}
	return result;
}

void SimpleDecorator::createFromJsonImpl(const rapidjson::Value& json)
{
	if (json.HasMember("slot"))
	{
		_slot = entt::hashed_string{ json["slot"].GetString() };
	}
}
