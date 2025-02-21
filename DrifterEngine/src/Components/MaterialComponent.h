#pragma once

#include <vector>
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct MaterialComponent
{
	float weight = 0.0f;
	std::vector<std::string> materials = {};

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Material";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<MaterialComponent, NAME>()
			.prop("serialize"_hs)
			.data<&MaterialComponent::weight>("weight"_hs)
			.data<&MaterialComponent::materials>("materials"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, MaterialComponent& material)
	{
		archive(material.weight, material.materials);
	}
}




