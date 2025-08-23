#pragma once

enum class AiTargetType
{
	None,
	Actors,
	Items
};

extern const std::unordered_map<AiTargetType, std::string_view> AiTargetTypeToString;
extern const std::unordered_map<std::string_view, AiTargetType> StringToAiTargetType;

namespace AiTargetTypes
{
	entt::id_type toIdHash(AiTargetType targetType);
}
