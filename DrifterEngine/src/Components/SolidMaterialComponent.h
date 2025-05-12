#pragma once

#include <string>
#include "Snapshot/Reflection.h"


struct SolidMaterialComponent
{
	float density = 0.f;
	float hardness = 0.f;
	float brittleness = 0.f;
	float malleability = 0.f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "solid_material";

	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<SolidMaterialComponent, NAME>()
			.prop("serialize"_hs)
			.data<&SolidMaterialComponent::density>("density"_hs)
			.data<&SolidMaterialComponent::hardness>("hardness"_hs)
			.data<&SolidMaterialComponent::brittleness>("brittleness"_hs)
			.data<&SolidMaterialComponent::malleability>("malleability"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, SolidMaterialComponent& properties)
	{
		archive(
			properties.density,
			properties.hardness, 
			properties.brittleness, 
			properties.malleability
		);
	}
}