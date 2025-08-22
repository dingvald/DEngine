#pragma once

enum class AiTargetTypes
{
	Actors,
	Items
};

static const std::unordered_map<AiTargetTypes, std::string_view> AiTargetTypeToString;
static const std::unordered_map<std::string_view, AiTargetTypes> StringToAiTargetType;