#include "pch.h"
#include "EntityFactory.h"
#include "Components/ComponentMetaBinder.h"
#include "Utility/CopyEntity.h"
#include <EnTT/meta/container.hpp>
#include "JSON/JSONHelpers.h"

#include "Components/InheritanceComponent.h"
#include "Components/PrototypeComponent.h"

#include <Utility/StandardLogger.h>

using namespace entt::literals;

static const char* COMPONENTS_KEY_NAME = "Components";
static const char* INHERITS_KEY_NAME = "Inherits";

drft::EntityFactory::EntityFactory()
{
	ComponentMetaBinder::bindComponents();
} 

bool drft::EntityFactory::loadPrototypes(const std::filesystem::path& directoryPath)
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
	{
		if (entry.is_directory()) continue;

		json::JsonRootExtractor jsonRootExtractor{ entry.path(), "Prototypes" };
		if (!jsonRootExtractor.isValid())
		{
			error_logger << "Error: " << entry << " could not be parsed." << std::endl;
		}
		else
		{
			for (auto&& node : jsonRootExtractor.getRoot().GetObject())
			{
				entt::entity entity = _protoRegistry.create();
				const std::string entityName = node.name.GetString();
				_prototypes[entityName] = entity;
				_prototypeNames[entity] = entityName;

				createEntitiyPrototypeFromJSON(entity, entityName, node.value);
			}
		}
	}

	resolvePrototypeInheritance();

	return true;
}

entt::const_handle drft::EntityFactory::get(const std::string& name) const
{
	if (!_prototypes.contains(name))
	{
		return entt::const_handle{ _protoRegistry, entt::null };
	}
	return entt::const_handle{ _protoRegistry, _prototypes.at(name) };
}

const std::string& drft::EntityFactory::getName(entt::entity prototype) const
{
	if (!_protoRegistry.valid(prototype)) throw std::exception("Entity does not belong to prototypes");
	if (!_prototypeNames.contains(prototype)) throw std::exception("Entity does not belong to prototypes"); // Something went wrong...
	return _prototypeNames.at(prototype);
}

const std::unordered_set<std::string> drft::EntityFactory::getFlattenedInheritance(entt::const_handle entity) const
{
	std::unordered_set<std::string> result;
	if (auto inheritance = entity.try_get<InheritanceComponent>())
	{
		for (auto&& base : inheritance->bases)
		{
			auto baseEntity = get(base);
			auto theirInheritance = getFlattenedInheritance(baseEntity);
			result.merge(theirInheritance);
		}
	}

	return result;
}

const entt::registry& drft::EntityFactory::prototypes() const
{
	return _protoRegistry;
}

entt::handle drft::EntityFactory::build(const std::string& name, entt::registry& registry) const
{
	if (!_prototypes.contains(name))
	{
		error_logger << "Error: Trying to create entity " << name << " but it does not exist in the prototype registry." << std::endl;
		return entt::handle{ registry, entt::null };
	}
	entt::entity newEntity = registry.create();
	util::copyEntity(newEntity, _prototypes.at(name), registry, _protoRegistry);

	registry.emplace<PrototypeComponent>(newEntity, name);

	return entt::handle(registry, newEntity);
}

bool drft::EntityFactory::has(const std::string& name) const
{
	return _prototypes.contains(name);
}

void drft::EntityFactory::resolvePrototypeInheritance()
{
	std::cout << "Resolving entity inheritance..." << std::endl;
	const int numEntitiesToResolve = _inheritanceQueue.size();
	std::cout << "Entities to resolve: " << numEntitiesToResolve << std::endl;

	int currentQueueSize = numEntitiesToResolve;
	int oldQueueSize = currentQueueSize;
	int iterations = 0;

	while (!_inheritanceQueue.empty())
	{
		auto& relationship = _inheritanceQueue.front();
		bool canResolve = false;
		if (relationship.bases.empty())
		{
			canResolve = true;
		}
		else
		{
			bool allBasesResolved = true;
			for (auto&& base : relationship.bases)
			{
				if (!_resolvedInheritance.contains(base))
				{
					allBasesResolved = false;
					break;
				}
			}
			if (allBasesResolved)
			{
				canResolve = true;
			}
		}

		if (canResolve)
		{
			auto entity = get(relationship.entityName);
			auto& inheritanceComp = _protoRegistry.emplace<InheritanceComponent>(entity.entity());
			for (auto&& base : relationship.bases)
			{
				auto baseEntity = get(base);
				util::copyEntity(entity.entity(), baseEntity.entity(), _protoRegistry, false);
				inheritanceComp.bases.insert(base);
			}
			_resolvedInheritance.insert(relationship.entityName);
		}
		else
		{
			_inheritanceQueue.push(relationship);
		}
		_inheritanceQueue.pop();

		currentQueueSize = _inheritanceQueue.size();
		if (oldQueueSize == currentQueueSize)
		{
			++iterations;
			if (iterations > currentQueueSize)
			{
				// we have a problem
				break;
			}
		}
		oldQueueSize = currentQueueSize;
	}

	if (_inheritanceQueue.empty())
	{
		std::cout << "All entities resolved." << std::endl;
	}
	else
	{
		error_logger << "Error: " << _inheritanceQueue.size() << " entities could not be resolved:" << std::endl;
		while (!_inheritanceQueue.empty())
		{
			auto& relationship = _inheritanceQueue.front();
			error_logger << relationship.entityName << std::endl;
			_inheritanceQueue.pop();
		}
	}
}

void drft::EntityFactory::createEntitiyPrototypeFromJSON(entt::entity entity, const std::string& entityName, const rapidjson::Value& json)
{
	const auto entityObject = json.GetObject();

	if (entityObject.HasMember(INHERITS_KEY_NAME))
	{
		InheritanceRelationship newRelationship;
		newRelationship.entityName = entityName;
		for (auto&& base : entityObject[INHERITS_KEY_NAME].GetArray())
		{
			auto baseName = std::string(base.GetString());
			if (baseName.empty()) continue;
			newRelationship.bases.emplace_back(std::move(baseName));
		}
		_inheritanceQueue.push(newRelationship);
	}
	else
	{
		_resolvedInheritance.insert(entityName);
	}

	if (entityObject.HasMember(COMPONENTS_KEY_NAME))
	{
		// Iterate each component
		for (auto&& component : entityObject[COMPONENTS_KEY_NAME].GetObject())
		{
			auto componentName = component.name.GetString();
			auto meta = entt::resolve(entt::hashed_string(componentName));
			auto any = meta.func("emplace"_hs).invoke(meta, entt::forward_as_meta(_protoRegistry), entity);

			// Allow the component to initialize itself using the json value
			if (auto setFromJsonFunc = meta.func("set_from_json"_hs))
			{
				setFromJsonFunc.invoke(any, entt::forward_as_meta(std::as_const(component.value)));
			}
			else
			{
				// Iterate component data
				for (auto&& data : component.value.GetObject())
				{
					const auto memberName = data.name.GetString();
					if (data.value.IsArray())
					{
						// HACKZZ: Inflexible - assumes certain types in arrays
						auto arr = data.value.GetArray();
						int size = arr.Size();
						if (size == 0) continue;
						if (size == 2 && arr[0].IsInt())
						{
							sf::Vector2i vec2 = { arr[0].GetInt(), arr[1].GetInt() };
							meta.data(entt::hashed_string(memberName)).set(any, vec2);
						}
						else if (size == 3 && arr[0].IsInt())
						{
							sf::Color col = {
								static_cast<std::uint8_t>(arr[0].GetInt()),
								static_cast<std::uint8_t>(arr[1].GetInt()),
								static_cast<std::uint8_t>(arr[2].GetInt())
							};
							meta.data(entt::hashed_string(memberName)).set(any, col);
						}
						else if (arr[0].IsString())
						{
							// Either a unordered set of strings or a vector of strings
							if (meta.data(entt::hashed_string(memberName)).type().is_associative_container())
							{
								std::unordered_set<std::string> strings;
								for (int i = 0; i < size; ++i)
								{
									strings.insert(arr[i].GetString());
								}
								meta.data(entt::hashed_string(memberName)).set(any, strings);
							}
							else if (meta.data(entt::hashed_string(memberName)).type().is_sequence_container())
							{
								std::vector<std::string> strings;
								for (int i = 0; i < size; ++i)
								{
									strings.push_back(arr[i].GetString());
								}
								meta.data(entt::hashed_string(memberName)).set(any, strings);
							}
						}
					}
					else if (data.value.IsString())
					{
						std::string val(data.value.GetString());
						meta.data(entt::hashed_string(memberName)).set(any, val);
					}
					else if (data.value.IsInt())
					{
						meta.data(entt::hashed_string(memberName)).set(any, data.value.GetInt());
					}
					else if (data.value.IsFloat())
					{
						meta.data(entt::hashed_string(memberName)).set(any, data.value.GetFloat());
					}
					else if (data.value.IsObject())
					{
						if (data.value.GetObject().MemberCount() == 0) continue;
						if (data.value.GetObject().begin()->value.IsInt())
						{
							if (auto varAny = meta.data(entt::hashed_string(memberName)).get(any).try_cast<std::unordered_map<std::string, int>>())
							{
								std::unordered_map<std::string, int> map;
								for (auto&& mapData : data.value.GetObject())
								{
									map.emplace(mapData.name.GetString(), mapData.value.GetInt());
								}

								meta.data(entt::hashed_string(memberName)).set(any, map);
							}
							else if (auto varAny = meta.data(entt::hashed_string(memberName)).get(any).try_cast<std::unordered_map<std::string, unsigned long>>())
							{
								std::unordered_map<std::string, unsigned long> map;
								for (auto&& mapData : data.value.GetObject())
								{
									map.emplace(mapData.name.GetString(), mapData.value.GetInt());
								}

								meta.data(entt::hashed_string(memberName)).set(any, map);
							}
						}
						else if (data.value.GetObject().begin()->value.IsFloat())
						{
							std::unordered_map<std::string, float> map;
							for (auto&& mapData : data.value.GetObject())
							{
								map.emplace(mapData.name.GetString(), mapData.value.GetFloat());
							}
							meta.data(entt::hashed_string(memberName)).set(any, map);
						}
						else if (data.value.GetObject().begin()->value.IsString())
						{
							std::unordered_map<std::string, std::string> map;
							for (auto&& mapData : data.value.GetObject())
							{
								map.emplace(mapData.name.GetString(), mapData.value.GetString());
							}
							meta.data(entt::hashed_string(memberName)).set(any, map);
						}
					}
					else if (data.value.IsBool())
					{
						meta.data(entt::hashed_string(memberName)).set(any, data.value.GetBool());
					}
				}
			}
		}
	}
}


