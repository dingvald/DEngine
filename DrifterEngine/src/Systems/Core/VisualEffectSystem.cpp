#include "pch.h"
#include "VisualEffectSystem.h"
#include "Components/PositionComponent.h"
#include "Components/VisualEffectComponent.h"
#include "Components/RenderComponent.h"
#include "Utility/Math.h"


void drft::system::VisualEffectSystem::onFixedUpdate()
{
	auto view = _registry.view<PositionComponent, VisualEffectComponent, RenderComponent>();
	for (auto [entity, pos, effect, render] : view.each())
	{
		// Effects with their ttl set to negative need to be destroyed manually
		if (effect.ttl < 0) continue; 

		--effect.ttl;
		if (effect.fades)
		{
			auto current_a = render.color.a;
			float delta = current_a / effect.ttl;
			render.color.a =  math::remap(0, 120, 0, 255, effect.ttl);
		}
		if (effect.ttl <= 0)
		{
			_registry.destroy(entity);
		}
	}
}
