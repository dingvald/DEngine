#pragma once

namespace drft
{
	/// <summary>
	/// Class that initializes and builds entities from prototypes.
	/// Prototypes are loaded from a JSON file located in the data/static folder.
	/// </summary>
	class EntityFactory
	{
	public:
		EntityFactory();

		// Load entity prototypes from the given JSON file into the given registry.
		bool loadPrototypes(std::string JSONfilename);

		// Build a new entity and place into the given registry.
		entt::handle build(const std::string& name, entt::registry& registry) const;

		// Check if the factory has a prototype with the given name.
		bool has(const std::string& name) const;

	private:
		entt::registry _protoRegistry;
		std::unordered_map<std::string, entt::entity> _prototypes;
	};
}



