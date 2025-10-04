#include "pch.h"
#include "JsonValueStorage.h"


JsonValueStorage::JsonValueStorage(const rapidjson::Value& value)
	: _doc(rapidjson::kObjectType)
{
	_doc.CopyFrom(value, _doc.GetAllocator());
}

const rapidjson::Value& JsonValueStorage::getValue() const
{
	return _doc;
}
