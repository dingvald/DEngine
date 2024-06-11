#include "pch.h"
#include "AnimationSystem.h"
#include "Engine/EngineConstants.h"
#include "Components/RenderComponent.h"
#include "Components/AnimationComponent.h"

#include "Utility/Math.h"

void drft::system::AnimationSystem::init()
{
}

void drft::system::AnimationSystem::fixedUpdate()
{
	math::Range<int> indexRange(0, 0);

	auto noRenderView = _registry->view<AnimationComponent>(entt::exclude<RenderComponent>);
	for (auto [entity, animation] : noRenderView.each())
	{
		_registry->emplace<RenderComponent>(entity, DebugRenderComponent);
	}

	auto withRenderView = _registry->view<AnimationComponent, RenderComponent>();
	for (auto [entity, animation, render] : withRenderView.each())
	{
		applySpriteOptionsToRenderComponent(render, animation.frames[animation.index]);

		++animation.elapsed;
		const float numFramesTillNextIndex = TARGET_FPS / std::abs(animation.speed);
		if (animation.elapsed >= numFramesTillNextIndex)
		{
			if (animation.speed > 0)
			{
				animation.index = moveToNextFrame(animation);
			}
			else if (animation.speed < 0)
			{
				animation.index = moveToPreviousFrame(animation);
			}

			indexRange.setMax(animation.frames.size() - 1);
			if (!indexRange.isValueWithinInclusive(animation.index))
			{
				_toRemoveAnimation.push_back(entity);	
			}

			animation.elapsed = animation.elapsed - numFramesTillNextIndex;
		}
	}
}

void drft::system::AnimationSystem::onFixedUpdateEnd()
{
	for (auto entity : _toRemoveAnimation)
	{
		_registry->remove<AnimationComponent>(entity);
	}
	_toRemoveAnimation.clear();
}

int drft::system::AnimationSystem::moveToNextFrame(const AnimationComponent& animation)
{
	int result = animation.index + 1;
	if (animation.loops && result >= animation.frames.size())
	{
		result = 0;
	}
	return result;
}

int drft::system::AnimationSystem::moveToPreviousFrame(const AnimationComponent& animation)
{
	int result = animation.index - 1;
	if (animation.loops && result < 0)
	{
		result = animation.frames.size() - 1;
	}
	return result;
}