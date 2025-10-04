#include "pch.h"
#include "SpawnEffect.h"

#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/VisualEffectComponent.h"

#include "Engine/EngineConstants.h"
#include <Utility/StandardLogger.h>

entt::entity drft::system::spawnEffect(entt::registry& registry, EffectStruct&& effect)
{
	if (effect.frames.empty()) throw std::exception("Cannot spawn effect with no sprites.");
	if (effect.animationSpeed > 0 && effect.ttl.has_value())
	{
		LOG_WARNING("Spawning effect with both a 'ttl' and an 'animationSpeed' - 'ttl' will have no effect");
	}

	entt::handle effectHandle = { registry, registry.create() };
	
	std::optional<float> ttl = effect.ttl;
	if (effect.frames.size() == 1)
	{
		auto& render = effectHandle.emplace<RenderComponent>();
		applySpriteOptionsToRenderComponent(render, effect.frames.front());
	}
	else if (effect.frames.size() > 1) // must be an animation
	{
		if (!ttl.has_value())
		{
			ttl = effect.frames.size() * effect.animationSpeed * SECONDS_PER_FRAME;
		}
		AnimationComponent animation = { .frames = effect.frames, .speed = effect.animationSpeed, .loops = effect.loops };
		effectHandle.emplace<AnimationComponent>(animation);
	}

	effectHandle.emplace<VisualEffectComponent>(ttl, effect.fadeFunc, effect.requiresInFOV);
	effectHandle.emplace<PositionComponent>(effect.position, effect.offset);
	
	return effectHandle.entity();
}
