#pragma once

namespace drft::util
{
	void saveRegistryToFile(const entt::registry& reg, const std::filesystem::path& filepath);
}