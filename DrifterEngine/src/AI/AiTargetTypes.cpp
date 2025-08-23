#include <pch.h>

#include "AiTargetTypes.h"

const std::unordered_map<AiTargetType, std::string_view> AiTargetTypeToString =
{
	{ AiTargetType::Actors, "actors" },
	{ AiTargetType::Items, "items" }
};

const std::unordered_map<std::string_view, AiTargetType> StringToAiTargetType =
{
	{ "actors", AiTargetType::Actors },
	{ "items", AiTargetType::Items }
};

entt::id_type AiTargetTypes::toIdHash(AiTargetType targetType)
{
	return entt::hashed_string{ AiTargetTypeToString.at(targetType).data()};
}
