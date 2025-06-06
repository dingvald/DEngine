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
	static inline const std::string_view NAME = "openable";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<OpenableComponent, NAME>(ctx)
			.data<&OpenableComponent::keyName>("key_name"_hs);
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


