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
		component::Animation animation = { .sprites = {1, 2}, .speed = 1.0f, .loops = true };
		registry->emplace<component::Animation>(entity, animation);
	}
}

void drft::system::AnimationSystem::fixedUpdate()
{
	auto view = registry->view<component::Render, component::Animation>();
	for (auto [entity, render, animation] : view.each())
	{
		animation.elapsed += SECONDS_PER_FRAME;
		if (animation.elapsed >= animation.speed)
		{
			animation.elapsed = animation.elapsed - animation.speed;
			render.sprite = animation.sprites[animation.index];
			++animation.index;
			if (animation.index >= animation.sprites.size())
			{
				if (animation.loops)
				{
					animation.index = 0;
				}
				else
				{
					_toRemoveAnimation.push_back(entity);
				}
			}
		}
	}
}

void drft::system::AnimationSystem::onFixedUpdateEnd()
{
	for (auto entity : _toRemoveAnimation)
	{
		registry->remove<component::Animation>(entity);
	}
}
