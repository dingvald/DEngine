#include "pch.h"
#include "EntityFactory.h"
#include "Components/ComponentMetaBinder.h"
#include "Utility/CopyEntity.h"
#include "JSON/JSONHelpers.h"

#include "Components/InheritanceComponent.h"
#include "Components/PrototypeComponent.h"

#include <Utility/StandardLogger.h>

using namespace entt::literals;

static const char* COMPONENTS_KEY_NAME = "components";
static const char* INHERITS_KEY_NAME = "inherits";

const entt::id_type NullEntity = "NULL_ENTITY"_hs;

drft::EntityFactory::EntityFactory()
{
	ComponentMetaBinder::bindComponents();
} 

bool drft::EntityFactory::loadPrototypes(const std::filesystem::path& directoryPath)
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
	{
		if (entry.is_directory()) continue;

		json::JsonFileWrapper json{ entry.path(), "prototypes" };
		if (!json.load())
		{
			LOG_ERROR("{} could not be loaded", entry.path().string());
			continue;
		}
		else
		{
			for (auto&& node : json.getRoot().GetObject())
			{
				entt::entity entity = _protoRegistry.create();
				entt::id_type entityId = entt::hashed_string{ node.name.GetString() };

				_prototypes.emplace(entityId, entity);
				_prototypeIDs.emplace(entity, entityId);

				createEntitiyPrototypeFromJSON(entity, entityId, node.value);
			}
		}
	}

	resolvePrototypeInheritance();

	return true;
}

entt::const_handle drft::EntityFactory::get(entt::id_type id) const
{
	if (!_prototypes.contains(id))
	{
		return entt::const_handle{ _protoRegistry, entt::null };
	}
	return entt::const_handle{ _protoRegistry, _prototypes.at(id) };
}

entt::id_type drft::EntityFactory::getId(entt::entity prototype) const
{
	if (!_prototypeIDs.contains(prototype))
	{
		return NullEntity;
	}
	return _prototypeIDs.at(prototype);
}

entt::id_type drft::EntityFactory::getId(entt::const_handle prototype) const
{
	return getId(prototype.entity());
}

std::unordered_set<entt::id_type> drft::EntityFactory::getFlattenedInheritance(entt::const_handle entity) const
{
	std::unordered_set<entt::id_type> result;
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

entt::handle drft::EntityFactory::build(entt::id_type id, entt::registry& registry) const
{
	if (!_prototypes.contains(id))
	{
		LOG_ERROR("Trying to create entity {} but it does not exist in the prototype registry", id);
		return entt::handle{ registry, entt::null };
	}
	entt::entity newEntity = registry.create();
	util::copyEntity(newEntity, _prototypes.at(id), registry, _protoRegistry);

	registry.emplace<PrototypeComponent>(newEntity, id);

	return entt::handle(registry, newEntity);
}

bool drft::EntityFactory::has(entt::id_type id) const
{
	return _prototypes.contains(id);
}

void drft::EntityFactory::resolvePrototypeInheritance()
{
	LOG_MSG("Resolving entity inheritance...");
	LOG_MSG("Entities to resolve: {}", _inheritanceQueue.size());

	int currentQueueSize = _inheritanceQueue.size();
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
			auto entity = get(relationship.entityId);
			auto& inheritanceComp = _protoRegistry.emplace<InheritanceComponent>(entity.entity());
			for (auto&& base : relationship.bases)
			{
				auto baseEntity = get(base);
				util::copyEntity(entity.entity(), baseEntity.entity(), _protoRegistry, false);
				inheritanceComp.bases.insert(base);
			}
			_resolvedInheritance.insert(relationship.entityId);
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
		LOG_MSG("All entities resolved.");
	}
	else
	{
		LOG_ERROR("{} entities could not be resolved", _inheritanceQueue.size());
	}
}

void drft::EntityFactory::createEntitiyPrototypeFromJSON(entt::entity entity, entt::id_type id, const rapidjson::Value& json)
{
	const auto entityObject = json.GetObject();

	if (entityObject.HasMember(INHERITS_KEY_NAME))
	{
		InheritanceRelationship newRelationship;
		newRelationship.entityId = id;
		for (auto&& base : entityObject[INHERITS_KEY_NAME].GetArray())
		{
			auto baseId = entt::hashed_string{ base.GetString() };
			newRelationship.bases.emplace_back(baseId);
		}
		_inheritanceQueue.push(newRelationship);
	}
	else
	{
		_resolvedInheritance.insert(id);
	}

	if (entityObject.HasMember(COMPONENTS_KEY_NAME))
	{
		// Iterate each component
		for (auto&& component : entityObject[COMPONENTS_KEY_NAME].GetObject())
		{
			auto componentName = component.name.GetString();
			auto meta = entt::resolve(entt::hashed_string(componentName));
			if (!meta)
			{
				LOG_ERROR("Component {} does not exist - has it been reflected?", componentName);
				continue;
			}
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


