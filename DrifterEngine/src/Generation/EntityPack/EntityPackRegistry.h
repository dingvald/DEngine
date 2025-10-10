#pragma once
#include <Generation/EntityPack/EntityPack.h>

class EntityPackRegistry
{
public:
	void loadEntityPacks(const std::filesystem::path& entityPackDirectory);

	const EntityPack& get(entt::id_type id) const;

private:
	std::unordered_map<entt::id_type, EntityPack> _entityPacks;
};