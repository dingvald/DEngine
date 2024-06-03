#pragma once

#include "Systems/HelperClasses/BodyParts.h"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct BodyComponent
{
	PartTree parts;
private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Body";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<BodyComponent, NAME>()
			.prop("serialize"_hs)
			.data<&BodyComponent::parts>("parts"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, BodyComponent& body)
	{
		archive(body.parts);
	}
}











