#pragma once

#include <string>
#include "Snapshot/Reflection.h"


struct GasMaterialComponent
{
	float density = 1.0f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "gas_material";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<GasMaterialComponent, NAME>(ctx)
			.data<&GasMaterialComponent::density>("density"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, GasMaterialComponent& gas)
	{
		archive(gas.density);
	}
}