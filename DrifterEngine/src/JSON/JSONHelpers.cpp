#include "pch.h"
#include "JSONHelpers.h"


drft::json::JsonRootExtractor::JsonRootExtractor(const std::filesystem::path& filepath, const char* rootObjectName)
	: _rootName(rootObjectName)
{
	_optionalDoc = extractDOM(filepath, rootObjectName);
}

bool drft::json::JsonRootExtractor::isValid() const
{
	return _optionalDoc.has_value();
}

rapidjson::Value& drft::json::JsonRootExtractor::getRoot()
{
	return _optionalDoc.value()[_rootName];
}

std::optional<rapidjson::Document> drft::json::JsonRootExtractor::extractDOM(const std::filesystem::path& filepath, const char* rootObjectName)
{
	using namespace rapidjson;

	std::ifstream ifs{ filepath };
	Document doc{};
	std::cout << "Loading " << filepath.filename() << "..." << std::endl;

	if (!ifs.is_open())
	{
		std::cerr << "Could not open file " << filepath << std::endl;
		return std::nullopt;
	}

	IStreamWrapper isw{ ifs };
	std::cout << "Parsing " << filepath.filename() << "..." << std::endl;
	doc.ParseStream<kParseCommentsFlag>(isw);

	if (doc.HasParseError())
	{
		std::cerr << filepath << " could not be parsed. Check file for format errors." << std::endl;
		return std::nullopt;
	}

	if (!doc.HasMember(rootObjectName))
	{
		std::cerr << filepath << " does not have the expected root object." << std::endl;
		return std::nullopt;
	}

	return doc;
}
