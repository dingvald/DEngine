#pragma once

namespace drft::json
{
	std::optional<rapidjson::Document> extractDOM(const std::filesystem::path& filepath, const char* rootObjectName);
}