#pragma once

#include "Snapshot/Reflection.h"


struct SharpComponent
{
	float value = 0.f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "sharp";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<SharpComponent, NAME>(ctx)
			.data<&SharpComponent::value>("value"_hs);
	}
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(value);
	}
};









