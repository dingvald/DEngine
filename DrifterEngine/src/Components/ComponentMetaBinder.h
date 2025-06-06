#pragma once
#include <EnTT/meta/context.hpp>

class ComponentMetaBinder
{
public:
	// Bind all entity components to the reflection context
	static void bindComponents();

	// Returns the reflection context that contains all reflected entity components
	static entt::meta_ctx& cxt();

private:
	static inline entt::meta_ctx _ctx = {};
};

