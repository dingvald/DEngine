#include "pch.h"
#include "DoorDecorator.h"
#include "Random/RandomNumberGenerator.h"
#include "Random/WeightedSelection.h"
#include "Structures/StructureShapeInstance.h"
#include "Spatial/Helpers.h"

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

		const auto choice = rng::weightedSelection(_entityWeights);
		if (choice < 0) continue;
		const auto& entityName = _entityWeights.at(choice).first;

		size_t index = rng::RandomNumberGenerator::intInRange(0, walls.size() - 1);
		auto it = walls.begin();
		std::advance(it, index);
		bool foundPosition = false;
		int iterations = 0;
		
		while (!foundPosition && iterations < walls.size())
		{
			sf::Vector2i position = *it;
			if (isPositionSuitable(position, shape))
			{
				foundPosition = true;
				shape.clearPosition(position);
				shape.setBit(position, StructureBit::Door);
				shape.addEntity(entityName, position);
			}
			else
			{
				it = std::next(it);
				++iterations;
				if (it == walls.end())
				{
					it = walls.begin();
				}
			}
		}
	}
}

bool drft::DoorDecorator::isPositionSuitable(sf::Vector2i position, const StructureShapeInstance& shape) const
{
	auto neighbors = spatial::getAdjacentPoints(position, spatial::AdjacentType::Cardinal);
	bool emptyFound = false;
	bool roomFound = false;

	for (auto&& neighbor : neighbors)
	{
		emptyFound = emptyFound || !shape.anyBits(neighbor);
		roomFound = roomFound || shape.checkBit(neighbor, StructureBit::Room);
	}

	return emptyFound && roomFound;
}
