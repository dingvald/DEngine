#include <pch.h>
#include "SingleFeature.h"

void SingleFeature::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("tag"))
	{
		_tag = entt::hashed_string{ json["tag"].GetString() };
	}
}

TaggedPositions SingleFeature::generateTags(sf::Vector3i position, const GenerationContext& context) const
{
	TaggedPositions result;
	PositionList newList;
	newList.emplace_back(position);
	result.emplace(_tag, std::move(newList));
	return result;
}