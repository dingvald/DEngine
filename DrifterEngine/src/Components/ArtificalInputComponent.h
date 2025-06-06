#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct ArtificalInputComponent
{

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "ai";
	static void bind(entt::meta_ctx& ctx)
	{
		snapshot::reflectComponent<ArtificalInputComponent, NAME>(ctx);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, ArtificalInputComponent& ai)
	{
		archive();
	}
}