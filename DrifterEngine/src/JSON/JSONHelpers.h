#pragma once

namespace drft::json
{
	class JsonRootExtractor
	{
	public:
		JsonRootExtractor(const std::filesystem::path& filepath, const char* rootObjectName);
		bool isValid() const;
		rapidjson::Value& getRoot();

	private:
		std::optional<rapidjson::Document> extractDOM(const std::filesystem::path& filepath, const char* rootObjectName);

	private:
		const char* _rootName;
		std::optional<rapidjson::Document> _optionalDoc;
	};
}