#pragma once

#include <vector>
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include <Cereal/external/rapidjson/document.h>

struct MaterialPercentage
{
	entt::id_type material;
	float percentage;
private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& ar)
	{
		ar(material, percentage);
	}
};

struct MaterialCompositionComponent
{
	std::vector<MaterialPercentage> materials = {};

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "material_composition";

	static void setFromJSON(MaterialCompositionComponent& composition, const rapidjson::Value& json)
	{
		for (auto&& val : json.GetObject())
		{
			MaterialPercentage newMat;
			newMat.material = entt::hashed_string{ val.name.GetString() };
			newMat.percentage = val.value.GetFloat();
			composition.materials.push_back(std::move(newMat));
		}
	}

	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<MaterialCompositionComponent, NAME>()
			.prop("serialize"_hs)
			.func<&MaterialCompositionComponent::setFromJSON>("set_from_json"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, MaterialCompositionComponent& material)
	{
		archive(material.materials);
	}
}