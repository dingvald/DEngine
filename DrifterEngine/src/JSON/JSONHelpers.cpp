#include "pch.h"
#include "JSONHelpers.h"

std::optional<rapidjson::Document>  drft::json::extractDOM(const std::filesystem::path& filepath, const char* rootObjectName)
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
