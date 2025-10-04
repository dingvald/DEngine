#pragma once

class JsonValueStorage
{
public:
	using Ptr = std::unique_ptr<JsonValueStorage>;

	JsonValueStorage(const rapidjson::Value& value);

	const rapidjson::Value& getValue() const;

private:
	rapidjson::Document _doc = {};
};