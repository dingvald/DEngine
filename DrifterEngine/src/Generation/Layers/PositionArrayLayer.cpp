#include "pch.h"
#include "PositionArrayLayer.h"
#include <Utility/StandardLogger.h>
#include <Spatial/ChunkPosition.h>

void drft::PositionArrayLayer::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("positions"))
	{
		for (auto&& val : json["positions"].GetArray())
		{
			if (!val.IsArray())
			{
				LOG_ERROR("'position_array' requires positions to be arrays");
				continue;
			}
			auto arr = val.GetArray();
			if (arr.Size() == 2)
			{
				sf::Vector2i pos;
				pos.x = arr[0].GetInt();
				pos.y = arr[1].GetInt();
				_positions2d.insert(pos);
			}
			else if (arr.Size() == 3)
			{
				sf::Vector3i pos;
				pos.x = arr[0].GetInt();
				pos.y = arr[1].GetInt();
				pos.z = arr[2].GetInt();
				_positions3d.insert(pos);
			}
		}
	}
}

double drft::PositionArrayLayer::getValueAt(sf::Vector3i tilePosition)
{
	if (_positions3d.contains(tilePosition)) return 1.0;
	if (_positions2d.contains(spatial::toXY(tilePosition))) return 1.0;
	return 0.0;
}
