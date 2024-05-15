#include "pch.h"
#include "DoorDecorator.h"
#include "Random/RandomNumberGenerator.h"
#include "Random/WeightedSelection.h"
#include "Structures/StructureShapeInstance.h"

void drft::DoorDecorator::createFromJSON(const rapidjson::Value& json)
{
	if (json.HasMember("Number"))
	{
		if (json["Number"].IsArray())
		{
			int min = json["Number"].GetArray()[0].GetInt();
			int max = json["Number"].GetArray()[1].GetInt();
			_numberOfDoors.setMin(min);
			_numberOfDoors.setMax(max);
		}
		else if (json["Number"].IsInt())
		{
			int number = json["Number"].GetInt();
			_numberOfDoors.setMin(number);
			_numberOfDoors.setMax(number);
		}
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

void drft::DoorDecorator::apply(StructureShapeInstance& shape) const
{
	int numberOfDoors = rng::RandomNumberGenerator::intInRange(_numberOfDoors.getMin(), _numberOfDoors.getMax());
	for (int i = 0; i < numberOfDoors; ++i)
	{
		const auto& walls = shape.getPositionsFor(StructureBit::Wall);
		if (walls.size() == 0) return;

		size_t index = rng::RandomNumberGenerator::intInRange(0, walls.size() - 1);
		auto it = walls.begin();
		std::advance(it, index);

		sf::Vector2i position = *it;
		shape.clearPosition(position);
		shape.setBit(position, StructureBit::Door);
		
		const auto choice = rng::weightedSelection(_entityWeights);
		if (choice < 0) continue;

		const auto& entityName = _entityWeights.at(choice).first;
		shape.addEntity(entityName, position);
	}
}
