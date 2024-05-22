#include "pch.h"
#include "EffectSystem.h"
#include "Components/Components.h"
#include "Utility/Math.h"



void drft::system::EffectSystem::init()
{
}

void drft::system::EffectSystem::fixedUpdate()
{
	auto view = _registry->view<component::Position, component::Effect, component::Render>();
	for (auto [entity, pos, effect, render] : view.each())
	{
		// Effects with their ttl set to negative need to be destroyed manually
		if (effect.ttl < 0) continue; 

		--effect.ttl;
		if (effect.fades)
		{
			render.color.a =  math::remap(0, 120, 0, 255, effect.ttl);
		}
		if (effect.ttl <= 0)
		{
			_registry->destroy(entity);
		}
	}
}
