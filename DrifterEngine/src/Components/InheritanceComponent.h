#pragma once

#include <unordered_set>
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct InheritanceComponent
{
	std::unordered_set<entt::id_type> bases;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "inheritance";
	static void bind(entt::meta_ctx& ctx)
	{
		snapshot::reflectComponent<InheritanceComponent, NAME>(ctx);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, InheritanceComponent& inheritance)
	{
		archive(inheritance.bases);
	}
}
