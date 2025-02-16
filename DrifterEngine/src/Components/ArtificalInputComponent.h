#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include <Actions/ActionTypeIds.h>

struct ArtificalInputComponent
{
	ActionID selectedAction = ActionID::None;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "AI";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<ArtificalInputComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, ArtificalInputComponent& ai)
	{
		archive(ai.selectedAction);
	}
}