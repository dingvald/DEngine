#pragma once
#include <string>
#include "Snapshot/Reflection.h"

struct BaseDamageComponent
{
	float min = 0.f;
	float max = 1.f;

private:
	friend class ComponentMetaBinder;

	static inline const std::string_view NAME = "base_damage";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<BaseDamageComponent, NAME>(ctx)
			.data<&BaseDamageComponent::min>("min"_hs)
			.data<&BaseDamageComponent::max>("max"_hs);
	}

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(min, max);
	}
};