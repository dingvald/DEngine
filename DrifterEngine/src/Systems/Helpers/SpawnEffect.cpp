#include "pch.h"
#include "SpawnEffect.h"
#include "Components/Components.h"
#include "Engine/EngineConstants.h"
#include "Systems/Rendering/RenderLayers.h"

void drft::system::spawnEffect(entt::registry& registry, EffectStruct&& effect)
{
	if (effect.sprites.empty()) throw std::exception("Cannot spawn effect with no sprites.");

	entt::handle effectHandle = { registry, registry.create() };
	
	effectHandle.emplace<component::Position>(effect.position);
	effectHandle.emplace<component::Render>(static_cast<unsigned int>(effect.sprites.front()), static_cast<unsigned int>(RenderLayer::Effects), effect.color);
	int ttl = effect.ttl;
	if (effect.sprites.size() > 1) // must be an animation
	{
		ttl = effect.loops ? ttl : (TARGET_FPS / effect.animationSpeed) * effect.sprites.size();
		component::Animation animation = { .sprites = effect.sprites, .speed = effect.animationSpeed, .loops = effect.loops };
		effectHandle.emplace<component::Animation>(animation);
	}
	effectHandle.emplace<component::Effect>(ttl, effect.fades);
}
