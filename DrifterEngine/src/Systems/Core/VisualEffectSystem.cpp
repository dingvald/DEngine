#include "pch.h"
#include "VisualEffectSystem.h"
#include "Components/PositionComponent.h"
#include "Components/VisualEffectComponent.h"
#include "Components/RenderComponent.h"


void drft::system::VisualEffectSystem::fixedUpdate()
{
	auto view = _registry.view<VisualEffectComponent, RenderComponent>();
	for (auto [entity, effect, render] : view.each())
	{
		// Effects with their ttl set to negative need to be destroyed manually
		if (effect.ttl < 0) continue; 

		--effect.ttl;
		render.color.a = std::clamp(render.color.a - effect.fadeRate, 0, 255);
		if (effect.ttl <= 0)
		{
			_registry.destroy(entity);
		}
	}
}
