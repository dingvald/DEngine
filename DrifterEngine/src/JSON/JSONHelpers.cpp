#include "pch.h"
#include "JSONHelpers.h"
#include <Utility/StandardLogger.h>

drft::json::JsonFileWrapper::JsonFileWrapper(const std::filesystem::path& path, const char* rootName)
	: _rootName(rootName)
	, _path(path)
{}

bool drft::json::JsonFileWrapper::load()
{
	using namespace rapidjson;

	std::ifstream ifs{ _path };
	Document doc{};
	std::cout << "Loading " << _path.filename() << "..." << std::endl;

	if (!ifs.is_open())
	{
		error_logger << "Could not open file " << _path << std::endl;
		return false;
	}

	IStreamWrapper isw{ ifs };
	std::cout << "Parsing " << _path.filename() << "..." << std::endl;
	doc.ParseStream<kParseCommentsFlag>(isw);

	if (doc.HasParseError())
	{
		error_logger << _path << " could not be parsed. Check file for format errors." << std::endl;
		return false;
	}

	if (!doc.HasMember(_rootName))
	{
		error_logger << _path << " does not have the expected root object " << "\"" << _rootName << "\"" << std::endl;
		return false;
	}

	_optionalDoc.emplace(std::move(doc));
	return true;
}

bool drft::json::JsonFileWrapper::create()
{
	rapidjson::Document doc;
	doc.SetObject();
	_optionalDoc.emplace(std::move(doc));
	_optionalDoc->AddMember(rapidjson::StringRef(_rootName), rapidjson::Value{ rapidjson::kObjectType }, _optionalDoc->GetAllocator());
	return true;
}

bool drft::json::JsonFileWrapper::loadOrCreate()
{
	if (load()) return true;

	std::cout << "Could not find file " << _path.filename() << std::endl;
	std::cout << "--- Will create DOM instead" << std::endl;

	return create();
}

const rapidjson::Value& drft::json::JsonFileWrapper::getRoot() const
{
	return _optionalDoc.value()[_rootName];
}

rapidjson::Value& drft::json::JsonFileWrapper::getRoot()
{
	return _optionalDoc.value()[_rootName];
}

rapidjson::Document::AllocatorType& drft::json::JsonFileWrapper::getAllocator()
{
	return _optionalDoc.value().GetAllocator();
}

bool drft::json::JsonFileWrapper::save()
{
	return save(_path);
}

bool drft::json::JsonFileWrapper::save(const std::filesystem::path& path)
{
	if (!_optionalDoc.has_value()) return false;

	std::filesystem::remove(path);

	std::ofstream ofs(path);
	rapidjson::OStreamWrapper osw(ofs);

	rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
	return _optionalDoc.value().Accept(writer);
}
