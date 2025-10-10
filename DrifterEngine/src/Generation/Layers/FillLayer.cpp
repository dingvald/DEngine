#include "pch.h"
#include "FillLayer.h"


double drft::FillLayer::getValueAt(sf::Vector3i tilePosition)
{
	return 1.0;
}

void drft::FillLayer::createFromJson(const rapidjson::Value& json)
{
}
