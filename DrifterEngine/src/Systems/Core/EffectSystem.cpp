#include "pch.h"
#include "EffectSystem.h"
#include "Components/Components.h"
#include "Utility/Math.h"



void drft::system::EffectSystem::init()
{
}

void drft::system::EffectSystem::fixedUpdate()
{
	auto view = registry->view<component::Position, component::Effect, component::Render>();
	for (auto [entity, pos, effect, render] : view.each())
	{
		--effect.ttl;
		if (effect.fades)
		{
			render.color.a =  math::remap(0, 120, 0, 255, effect.ttl);
		}
		if (effect.ttl <= 0)
		{
			_toDestroy.push_back(entity);
		}
	}
}

void drft::system::EffectSystem::onFixedUpdateEnd()
{
	for (auto entity : _toDestroy)
	{
		registry->destroy(entity);
	}
}
