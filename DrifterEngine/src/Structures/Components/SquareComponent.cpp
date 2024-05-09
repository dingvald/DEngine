#include "pch.h"
#include "SquareComponent.h"

void drft::SquareComponent::apply(std::unordered_map<entt::entity, PositionList>& layout) const
{
}

void drft::SquareComponent::createFromJSON(const rapidjson::Value& json)
{
	if (json.HasMember("Origin"))
	{
		if (json["Origin"].IsArray())
		{
			_origin.x = json["Origin"].GetArray()[0].GetInt();
			_origin.y = json["Origin"].GetArray()[1].GetInt();
		}
	}
	if (json.HasMember("Width"))
	{
		int min = 1;
		int max = 1;
		if (json["Width"].IsArray())
		{
			min = json["Width"].GetArray()[0].GetInt();
			max = json["Width"].GetArray()[1].GetInt();
			
		}
		else if (json["Width"].IsInt())
		{
			min = json["Width"].GetInt();
			max = min;
		}
		_width.setMin(min);
		_width.setMax(max);
	}
	if (json.HasMember("Height"))
	{
		int min = 1;
		int max = 1;
		if (json["Height"].IsArray())
		{
			min = json["Height"].GetArray()[0].GetInt();
			max = json["Height"].GetArray()[1].GetInt();

		}
		else if (json["Height"].IsInt())
		{
			min = json["Height"].GetInt();
			max = min;
		}
		_height.setMin(min);
		_height.setMax(max);
	}
	if (json.HasMember("Outline"))
	{
		if (json["Outline"].IsObject())
		{
			auto outlineObj = json["Outline"].GetObject();
			if (outlineObj.HasMember("Integrity"))
			{
				if (outlineObj["Integrity"].IsFloat())
				{
					_outlineIntegrity = outlineObj["Integrity"].GetFloat();
				}
			}
			if (outlineObj.HasMember("Entities"))
			{
				if (outlineObj["Entities"].IsArray())
				{
					for (auto&& entityWeightPair : outlineObj["Entities"].GetArray())
					{
						auto entityName = entityWeightPair.GetArray()[0].GetString();
						int weight = entityWeightPair.GetArray()[1].GetInt();
						_outlineEntityWeights.emplace_back(std::make_pair(entityName, weight));
					}
				}
			}
		}
	}
	if (json.HasMember("Fill"))
	{
		if (json["Fill"].IsObject())
		{
			auto fillObj = json["Fill"].GetObject();
			if (fillObj.HasMember("Integrity"))
			{
				if (fillObj["Integrity"].IsFloat())
				{
					_fillIntegrity = fillObj["Integrity"].GetFloat();
				}
			}
			if (fillObj.HasMember("Entities"))
			{
				if (fillObj["Entities"].IsArray())
				{
					for (auto&& entityWeightPair : fillObj["Entities"].GetArray())
					{
						auto entityName = entityWeightPair.GetArray()[0].GetString();
						int weight = entityWeightPair.GetArray()[1].GetInt();
						_fillEntityWeights.emplace_back(std::make_pair(entityName, weight));
					}
				}
			}
		}
	}
}
