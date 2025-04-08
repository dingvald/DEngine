#pragma once
#include <JSON/ICreateFromJson.h>

class BiomeFeature : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

};