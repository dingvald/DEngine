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
				pos.x = arr[0].GetInt() / ChunkDimensions.x;
				pos.y = arr[1].GetInt() / ChunkDimensions.y;
				_positions2d.insert(pos);
			}
			else if (arr.Size() == 3)
			{
				sf::Vector3i pos;
				pos.x = arr[0].GetInt() / ChunkDimensions.x;
				pos.y = arr[1].GetInt() / ChunkDimensions.y;
				pos.z = arr[2].GetInt() / ChunkDimensions.z;
				_positions3d.insert(pos);
			}
		}
	}
}

double drft::PositionArrayLayer::getValueAt(sf::Vector3i tilePosition)
{
	const sf::Vector3i chunkPosition = tilePosition.componentWiseDiv(ChunkDimensions);
	if (_positions3d.contains(chunkPosition)) return 1.0;
	if (_positions2d.contains(spatial::toXY(chunkPosition))) return 1.0;
	return 0.0;
}
