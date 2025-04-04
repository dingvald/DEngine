#pragma once
#include <JSON/ICreateFromJson.h>

class Generator : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;


private:

};

