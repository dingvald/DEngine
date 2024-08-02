#pragma once
#include <Cereal/external/rapidjson/document.h>

class ICreateFromJson
{
public:
	// Initializes the values of class members from JSON
	virtual void createFromJson(const rapidjson::Value& json) = 0;
};