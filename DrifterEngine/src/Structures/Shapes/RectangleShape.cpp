#include "pch.h"
#include "RectangleShape.h"
#include "Random/RandomNumberGenerator.h"
#include "Random/PercentChance.h"
#include "Spatial/Helpers.h"
#include "Structures/StructureShapeInstance.h"

using namespace entt::literals;

void drft::RectangleShape::createFromJSON(const rapidjson::Value& json)
{
	if (json.HasMember("width"))
	{
		int min = 1;
		int max = 1;
		if (json["width"].IsArray())
		{
			min = json["width"].GetArray()[0].GetInt();
			max = json["width"].GetArray()[1].GetInt();
			
		}
		else if (json["width"].IsInt())
		{
			min = json["width"].GetInt();
			max = min;
		}
		_width.setMin(min);
		_width.setMax(max);
	}
	if (json.HasMember("height"))
	{
		int min = 1;
		int max = 1;
		if (json["height"].IsArray())
		{
			min = json["height"].GetArray()[0].GetInt();
			max = json["height"].GetArray()[1].GetInt();

		}
		else if (json["height"].IsInt())
		{
			min = json["height"].GetInt();
			max = min;
		}
		_height.setMin(min);
		_height.setMax(max);
	}
	if (json.HasMember("outline"))
	{
		for (auto&& tag : json["outline"].GetArray())
		{
			_outlineTags.insert(entt::hashed_string{ tag.GetString() });
		}
	}
	if (json.HasMember("fill"))
	{
		for (auto&& tag : json["fill"].GetArray())
		{
			_fillTags.insert(entt::hashed_string{ tag.GetString() });
		}
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
			shape.setTag("reserved"_hs, {x, y});
			if (y == 0 || x == 0 || y == height - 1 || x == width - 1)
			{
				for (auto&& tag : _outlineTags)
				{
					shape.setTag(tag, { x, y });
				}
			}
			else
			{
				for (auto&& tag : _fillTags)
				{
					shape.setTag(tag, { x, y });
				}
			}
		}
	}
}
