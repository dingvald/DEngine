#include <Generation/Structures/Template/StructureTemplate.h>
#include <unordered_map>
#include <filesystem>
#include <entt/entt.hpp>

struct GenerationRegistries;

class StructureRegistry
{
public:
	void loadStructures(const std::filesystem::path& structuresDirectory);
	void finalize(const GenerationFinalizationContext& context);

	const StructureTemplate& get(entt::id_type id) const;

private:
	std::unordered_map<entt::id_type, StructureTemplate> _structures;
};
