#include <ProcGen/Prefabs/Prefab.h>

class PrefabRegistry
{
public:
	void loadPrefabs(const std::filesystem::path& prefabsDirectory);
	const Prefab& get(entt::id_type id) const;

private:
	std::unordered_map<entt::id_type, Prefab> _prefabs;
};