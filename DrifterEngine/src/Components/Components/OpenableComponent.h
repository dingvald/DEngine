#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct OpenableComponent
{
	bool isOpen = false;
	std::string keyName = "";

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Openable";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<OpenableComponent, NAME>()
			.prop("serialize"_hs)
			.data<&OpenableComponent::keyName>("keyName"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, OpenableComponent& openable)
	{
		archive(openable.isOpen, openable.keyName);
	}
}


