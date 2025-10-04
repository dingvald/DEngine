#include "pch.h"
#include "VisualEffectSystem.h"
#include "Components/VisualEffectComponent.h"
#include "Components/RenderComponent.h"
#include "Engine/EngineConstants.h"
#include <Utility/Math.h>

#pragma optimize("", off)


void drft::system::VisualEffectSystem::fixedUpdate()
{
	auto view = _registry.view<VisualEffectComponent, RenderComponent>();
	for (auto [entity, effect, render] : view.each())
	{
		if (!effect.ttl.has_value()) continue; 

		effect.elapsed += SECONDS_PER_FRAME;
		if (effect.elapsed > effect.ttl.value())
		{
			_registry.destroy(entity);
			continue;
		}

		const float progress = std::clamp(effect.fadeFunc(effect.elapsed / effect.ttl.value()), 0.0f, 1.0f);

		render.color.a = math::remap(0.0, 1.0, 0, 255.0, progress);
	}
}

void drft::system::VisualEffectSystem::shutdown()
{
	// Destroy all entities with the visual effect component
	auto view = _registry.view<VisualEffectComponent>();
	_registry.destroy(view.begin(), view.end());
}
