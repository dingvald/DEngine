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
	auto view = _registry->view<AnimationComponent>();
	for (auto [entity, animation] : view.each())
	{
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

			indexRange.setMax(animation.sprites.size() - 1);
			if (indexRange.isValueWithinInclusive(animation.index))
			{
				animation.elapsed = animation.elapsed - numFramesTillNextIndex;
				_registry->emplace_or_replace<RenderComponent>(entity, animation.sprites[animation.index]);
			}
			else
			{
				_toRemoveAnimation.push_back(entity);
			}
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
	if (animation.loops && result >= animation.sprites.size())
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
		result = animation.sprites.size() - 1;
	}
	return result;
}