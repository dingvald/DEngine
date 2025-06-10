#include <ProcGen/Structures/PrefabStructure.h>

class PrefabStructureRegistry
{
public:
	void loadStructures(const std::filesystem::path& structuresDirectory);
	const PrefabStructure& get(entt::id_type id) const;

private:
	std::unordered_map<entt::id_type, PrefabStructure> _structures;
};