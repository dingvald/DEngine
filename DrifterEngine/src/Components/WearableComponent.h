#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct WearableComponent
{
	std::vector<std::string> slots;
	std::unordered_map<std::string, int> protections;
private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "wearable";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<WearableComponent, NAME>(ctx)
			.data<&WearableComponent::slots>("slots"_hs)
			.data<&WearableComponent::protections>("protections"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, WearableComponent& wearable)
	{
		archive(wearable.slots, wearable.protections);
	}
}












