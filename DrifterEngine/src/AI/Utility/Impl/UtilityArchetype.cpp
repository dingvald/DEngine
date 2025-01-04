#include "pch.h"
#include "UtilityArchetype.h"

#include <Utility/StandardLogger.h>

void UtilityArchetype::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("actions"))
	{
		for (auto&& action : json["actions"].GetArray())
		{
			UtilityAction newAction;
			newAction.createFromJson(action);
			_actions.push_back(std::move(newAction));
		}
	}
	else
	{
		error_logger << "Error: Could not parse ai archetype - Requires \"action\" member" << std::endl;
	}
}
