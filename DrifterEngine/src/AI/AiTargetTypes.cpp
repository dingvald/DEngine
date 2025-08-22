#include <pch.h>

#include "AiTargetTypes.h"

const std::unordered_map<AiTargetTypes, std::string_view> AiTargetTypeToString =
{
	{ AiTargetTypes::Actors, "actors" },
	{ AiTargetTypes::Items, "items" }
};

const std::unordered_map<std::string_view, AiTargetTypes> StringToAiTargetType =
{

};