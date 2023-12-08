#include "pch.h"
#include "AnimationSystem.h"
#include "Engine/EngineConstants.h"
#include "Components/Components.h"

void drft::system::AnimationSystem::init()
{
}

void drft::system::AnimationSystem::onStart(bool isNewgame)
{
	// for testing!
	auto playerView = registry->view<component::Player>();
	for (auto entity : playerView)
	{
		component::Animation animation = { .sprites = {1, 2, 3, 4, 5, 6}, .speed = 1.0f, .loops = false };
		registry->emplace<component::Animation>(entity, animation);
	}
}

void drft::system::AnimationSystem::fixedUpdate()
{
	auto view = registry->view<component::Render, component::Animation>();
	for (auto [entity, render, animation] : view.each())
	{
		++animation.elapsed;
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
				if (animation.index == animation.sprites.size() - 1)
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
		registry->remove<component::Animation>(entity);
	}
}
