#include "pch.h"
#include "SpawnEffect.h"

#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/VisualEffectComponent.h"

#include "Engine/EngineConstants.h"
#include "Systems/Rendering/RenderLayers.h"

entt::entity drft::system::spawnEffect(entt::registry& registry, EffectStruct&& effect)
{
	if (effect.sprites.empty()) throw std::exception("Cannot spawn effect with no sprites.");

	entt::handle effectHandle = { registry, registry.create() };
	
	effectHandle.emplace<PositionComponent>(effect.position);
	effectHandle.emplace<RenderComponent>(static_cast<unsigned int>(effect.sprites.front()), static_cast<unsigned int>(effect.layer), effect.color);
	int ttl = effect.ttl;
	if (effect.sprites.size() > 1) // must be an animation
	{
		ttl = effect.loops ? ttl : (TARGET_FPS / effect.animationSpeed) * effect.sprites.size();
		AnimationComponent animation = { .sprites = effect.sprites, .speed = effect.animationSpeed, .loops = effect.loops };
		effectHandle.emplace<AnimationComponent>(animation);
	}
	effectHandle.emplace<VisualEffectComponent>(ttl, effect.fades, effect.requiresInFOV);
	return effectHandle.entity();
}
