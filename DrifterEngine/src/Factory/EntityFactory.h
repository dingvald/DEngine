#pragma once

namespace drft
{
	static const entt::id_type NullEntity;

	/// <summary>
	/// Class that initializes and builds entities from prototypes.
	/// Prototypes are loaded from a JSON file located in the data/static folder.
	/// </summary>
	class EntityFactory
	{
	public:
		EntityFactory();

		// Load all entity prototype files in the given directory into the prototype registry.
		bool loadPrototypes(const std::filesystem::path& directoryPath);

		// Returns the entity prototype with the given id. Returns entt::null if not found.
		entt::const_handle get(entt::id_type id) const;

		// Returns the identifier for the prototype entity
		entt::id_type getId(entt::entity prototype) const;
		// Returns the identifier for the prototype entity handle
		entt::id_type getId(entt::const_handle prototype) const;

		std::unordered_set<entt::id_type> getFlattenedInheritance(entt::const_handle entity) const;

		// Access the prototype registry (read-only).
		const entt::registry& prototypes() const;

		// Build a new entity and place into the given registry.
		entt::handle build(entt::id_type id, entt::registry& registry) const;

		// Check if the factory has a prototype with the given name.
		bool has(entt::id_type id) const;

	private:
		void resolvePrototypeInheritance();
		void createEntitiyPrototypeFromJSON(entt::entity entity, entt::id_type id, const rapidjson::Value& json);

	private:
		entt::registry _protoRegistry;
		std::unordered_map<entt::id_type, entt::entity> _prototypes;
		std::unordered_map<entt::entity, entt::id_type> _prototypeIDs;
		
		struct InheritanceRelationship
		{
			entt::id_type entityId;
			std::vector<entt::id_type> bases;
		};
		std::queue<InheritanceRelationship> _inheritanceQueue;
		std::unordered_set<entt::id_type> _resolvedInheritance;	
	};
}



