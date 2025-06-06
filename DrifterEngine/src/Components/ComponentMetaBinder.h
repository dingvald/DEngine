#pragma once
#include <EnTT/meta/context.hpp>

class ComponentMetaBinder
{
public:
	static void bindComponents();
	static entt::meta_ctx& cxt();

private:
	static inline entt::meta_ctx _ctx = {};
};

