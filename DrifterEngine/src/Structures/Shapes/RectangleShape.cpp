#include "pch.h"
#include "RectangleShape.h"
#include "Random/RandomNumberGenerator.h"
#include "Random/PercentChance.h"
#include "Spatial/Helpers.h"
#include "Structures/StructureShapeInstance.h"

void drft::RectangleShape::createFromJSON(const rapidjson::Value& json)
{
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
}

void drft::RectangleShape::doGenerate(StructureShapeInstance& shape)
{
	const int width = rng::RandomNumberGenerator::intInRange(_width.getMin(), _width.getMax());
	const int height = rng::RandomNumberGenerator::intInRange(_height.getMin(), _height.getMax());
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			shape.setBit({ x, y }, StructureBit::Reserved);
			if (y == 0 || x == 0
				|| y == height - 1 || x == width - 1)
			{
				shape.setBit({ x, y }, StructureBit::Wall);
			}
			else
			{
				shape.setBit({ x, y }, StructureBit::Floor);
				shape.setBit({ x, y }, StructureBit::Room);
			}
		}
	}
}
