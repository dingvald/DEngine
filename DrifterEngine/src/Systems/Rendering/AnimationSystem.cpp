#include "pch.h"
#include "AnimationSystem.h"
#include "Engine/EngineConstants.h"
#include "Components/RenderComponent.h"
#include "Components/AnimationComponent.h"

void drft::system::AnimationSystem::init()
{
}

void drft::system::AnimationSystem::fixedUpdate()
{
	auto view = _registry->view<RenderComponent, AnimationComponent>();
	for (auto [entity, render, animation] : view.each())
	{
		++animation.elapsed;
		// TODO: account for negative animation speeds so the animation goes backwards
		const float numFramesTillNextIndex = TARGET_FPS / std::abs(animation.speed);
		if (animation.elapsed >= numFramesTillNextIndex)
		{
			if (animation.loops)
			{
				animation.index = (animation.index + 1) % animation.sprites.size();
			}
			else
			{
				animation.index = std::min(animation.index + 1, static_cast<int>(animation.sprites.size()) - 1);
				if (animation.index >= animation.sprites.size() - 1)
				{
					_toRemoveAnimation.push_back(entity);
				}
			}
			animation.elapsed = animation.elapsed - numFramesTillNextIndex;
		}
		render.sprite = animation.sprites[animation.index];
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
