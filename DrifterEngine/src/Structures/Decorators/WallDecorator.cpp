#include "pch.h"
#include "WallDecorator.h"
#include "Structures/StructureShapeInstance.h"
#include "Random/WeightedSelection.h"
#include "Random/PercentChance.h"

void drft::WallDecorator::createFromJSON(const rapidjson::Value& json)
{
	if (json.HasMember("Integrity"))
	{
		_integrity = json["Integrity"].GetFloat();
	}
	if (json.HasMember("Entities"))
	{
		if (json["Entities"].IsArray())
		{
			for (auto&& entry : json["Entities"].GetArray())
			{
				std::string entityName = entry.GetArray()[0].GetString();
				int weight = entry.GetArray()[1].GetInt();
				_entityWeights.emplace_back(std::move(entityName), weight);
			}
		}
	}
}

void drft::WallDecorator::apply(StructureShapeInstance& shape) const
{
	for (auto&& position : shape.getPositionsFor(StructureBit::Wall))
	{
		if (!rng::percentChance(_integrity * 100)) continue;

		const auto choice = rng::weightedSelection(_entityWeights);
		if (choice < 0) continue;

		const auto& entityName = _entityWeights.at(choice).first;
		shape.addEntity(entityName, position);
	}
}
