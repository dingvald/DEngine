#include <ProcGen/Structures/Template/StructureTemplate.h>
#include <unordered_map>
#include <filesystem>
#include <entt/entt.hpp>

class StructureRegistry
{
public:
	void loadStructures(const std::filesystem::path& structuresDirectory);
	const StructureTemplate& get(entt::id_type id) const;

private:
	std::unordered_map<entt::id_type, StructureTemplate> _structures;
};
