#pragma once

namespace drft::json
{
	class JsonFileWrapper
	{
	public:
		JsonFileWrapper(const std::filesystem::path& path, const char* rootName);

		bool load();
		bool create();
		bool loadOrCreate();
		const rapidjson::Value& getRoot() const;
		rapidjson::Value& getRoot();
		rapidjson::Document::AllocatorType& getAllocator();
		bool save();
		bool save(const std::filesystem::path& path);

	private:
		std::filesystem::path _path;
		const char* _rootName;
		std::optional<rapidjson::Document> _optionalDoc;
	};
}