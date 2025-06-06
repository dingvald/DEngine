#pragma once

#include <string>
#include "Snapshot/Reflection.h"


struct LiquidMaterialComponent
{
	float density = 0.f;
	float viscosity = 0.f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "liquid_material";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<LiquidMaterialComponent, NAME>(ctx)
			.data<&LiquidMaterialComponent::density>("density"_hs)
			.data<&LiquidMaterialComponent::viscosity>("viscosity"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, LiquidMaterialComponent& liquid)
	{
		archive(liquid.viscosity);
	}
}





