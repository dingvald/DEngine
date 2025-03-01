#pragma once

#include <AI/Utility/IUtilityInputProvider.h>
#include <AI/Utility/IUtilityBlackboard.h>
#include <AI/Utility/UtilityActionTargetPair.h>

#include <AI/Utility/Impl/UtilityAISharedTypes.h>
#include <AI/Utility/Impl/UtilityArchetype.h>

#include <JSON/JSONHelpers.h>
#include <Utility/StandardLogger.h>


template<typename EntityType>
class UtilityAI
{	
public:
	using ScoredActions = std::multimap<float, UtilityActionTargetPair<EntityType>>;

public:
	void loadUtilityArchetypes(const std::filesystem::path& directory)
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			if (entry.is_directory()) continue;

			drft::json::JsonFileWrapper json{ entry.path(), "ai_archetype" };
			if (!json.load())
			{
				error_logger << "Error: " << entry << " could not be loaded." << std::endl;
			}
			else
			{
				auto arch = json.getRoot().GetObject();
				if (arch.HasMember("name"))
				{
					auto name = arch["name"].GetString();
					UtilityArchetype newArchetype;
					newArchetype.createFromJson(json.getRoot());
					_archetypes.emplace(entt::hashed_string{ name }, std::move(newArchetype));
				}
				else
				{
					error_logger << "Error: Could not parse " << entry.path().filename() << " - \"name\" member required" << std::endl;
				}
			}
		}
	}

	void setInputProvider(const IUtilityInputProvider<EntityType>& inputProvider)
	{
		_inputProvider = &inputProvider;
	}

	ScoredActions scoreActions(EntityType entity, UtilityArchetypeID id, const IUtilityBlackboard<EntityType>& blackboard) const
	{
		if (!_inputProvider)
		{
			throw std::exception("Cannot use the utility AI without a input provider");
		}

		auto& archetype = _archetypes.at(id);
		return archetype.scoreActions(entity, blackboard, *_inputProvider);
	}

private:
	std::unordered_map< UtilityArchetypeID, UtilityArchetype > _archetypes;
	const IUtilityInputProvider<EntityType>* _inputProvider = nullptr;
};
