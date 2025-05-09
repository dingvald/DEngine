#include "pch.h"
#include "SpawnEffect.h"

#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/VisualEffectComponent.h"

#include "Engine/EngineConstants.h"

entt::entity drft::system::spawnEffect(entt::registry& registry, EffectStruct&& effect)
{
	if (effect.frames.empty()) throw std::exception("Cannot spawn effect with no sprites.");

	entt::handle effectHandle = { registry, registry.create() };
	
	int ttl = effect.ttl;
	if (effect.frames.size() == 1)
	{
		auto& render = effectHandle.emplace<RenderComponent>();
		applySpriteOptionsToRenderComponent(render, effect.frames.front());
	}
	else if (effect.frames.size() > 1) // must be an animation
	{
		ttl = effect.loops ? ttl : (TARGET_UPDATES_PER_SECOND / effect.animationSpeed) * effect.frames.size();
		AnimationComponent animation = { .frames = effect.frames, .speed = effect.animationSpeed, .loops = effect.loops };
		effectHandle.emplace<AnimationComponent>(animation);
	}

	effectHandle.emplace<PositionComponent>(effect.position, effect.offset);
	effectHandle.emplace<VisualEffectComponent>(ttl, effect.fadeRate, effect.requiresInFOV);
	
	return effectHandle.entity();
}
