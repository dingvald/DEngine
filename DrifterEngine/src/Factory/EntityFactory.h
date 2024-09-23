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

		// Load entity prototypes from the given JSON file into the prototype registry.
		bool loadPrototypes(const std::filesystem::path& directoryPath);

		// Returns the entity prototype with the given name. Returns entt::null if not found.
		entt::const_handle get(const std::string& name) const;

		const std::string& getName(entt::entity prototype) const;

		const std::unordered_set<std::string> getFlattenedInheritance(entt::const_handle entity) const;

		// Access the prototype registry (read-only).
		const entt::registry& prototypes() const;

		// Build a new entity and place into the given registry.
		entt::handle build(const std::string& name, entt::registry& registry) const;

		// Check if the factory has a prototype with the given name.
		bool has(const std::string& name) const;

	private:
		void resolvePrototypeInheritance();
		void createEntitiyPrototypeFromJSON(entt::entity entity, const std::string& entityName, const rapidjson::Value& json);

	private:
		entt::registry _protoRegistry;
		std::unordered_map<std::string, entt::entity> _prototypes;
		std::unordered_map<entt::entity, std::string> _prototypeNames;
		
		struct InheritanceRelationship
		{
			std::string entityName;
			std::vector<std::string> bases;
		};
		std::queue<InheritanceRelationship> _inheritanceQueue;
		std::unordered_set<std::string> _resolvedInheritance;	
	};
}



