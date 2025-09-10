#include "pch.h"
#include "AnimationSystem.h"
#include "Engine/EngineConstants.h"
#include "Components/RenderComponent.h"
#include "Components/AnimationComponent.h"

#include "Utility/Math.h"

void drft::system::AnimationSystem::fixedUpdate()
{
	auto noRenderView = _registry.view<AnimationComponent>(entt::exclude<RenderComponent>);
	for (auto&& [entity, animation] : noRenderView.each())
	{
		_registry.emplace<RenderComponent>(entity, DebugRenderComponent);
	}

	auto withRenderView = _registry.view<AnimationComponent, RenderComponent>();
	for (auto&& [entity, animation, render] : withRenderView.each())
	{
		++animation.elapsed;
		const float numFramesTillNextIndex = TARGET_UPDATES_PER_SECOND / std::abs(animation.speed);
		if (animation.elapsed >= numFramesTillNextIndex)
		{
			animation.index = math::wrap(animation.index + math::sign(animation.speed), 0, animation.frames.size() - 1);
			if (!animation.loops && ((animation.index == animation.frames.size() - 1) || animation.index == 0))
			{
				_toRemoveAnimation.emplace_back(entity);
			}

			animation.elapsed -= numFramesTillNextIndex;
		}

		applySpriteOptionsToRenderComponent(render, animation.frames[animation.index]);
	}
}

void drft::system::AnimationSystem::fixedUpdateEnd()
{
	for (auto entity : _toRemoveAnimation)
	{
		_registry.remove<AnimationComponent>(entity);
	}
	_toRemoveAnimation.clear();
}