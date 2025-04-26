#pragma once

#include <string>
#include "Snapshot/Reflection.h"


struct MaterialPropertiesComponent
{
	float hardness = 0.f;
	float brittleness = 0.f;
	float malleability = 0.f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "material_properties";

	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<MaterialPropertiesComponent, NAME>()
			.prop("serialize"_hs)
			.data<&MaterialPropertiesComponent::hardness>("hardness"_hs)
			.data<&MaterialPropertiesComponent::brittleness>("brittleness"_hs)
			.data<&MaterialPropertiesComponent::malleability>("malleability"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, MaterialPropertiesComponent& properties)
	{
		archive(
			properties.hardness, 
			properties.brittleness, 
			properties.malleability
		);
	}
}