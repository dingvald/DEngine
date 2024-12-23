#include "pch.h"
#include "SyncedAnimationSystem.h"
#include "Engine/EngineConstants.h"
#include "Components/SyncedAnimationComponent.h"
#include "Components/RenderComponent.h"
#include "Components/Tags.h"


void drft::system::SyncedAnimationSystem::update()
{
	updateSyncPoints();

	auto view = _registry.view<SyncedAnimationComponent, component::tag::InPlayerFOV>();
	for (auto&& [entity, animation] : view.each())
	{
		int index = _syncPoints[convertFloatToIntDec(animation.speed)].index % animation.frames.size();
		if (auto render = _registry.try_get<RenderComponent>(entity))
		{
			applySpriteOptionsToRenderComponent(*render, animation.frames[index]);
		}
		else
		{
			auto& newRender = _registry.emplace<RenderComponent>(entity, DebugRenderComponent);
			applySpriteOptionsToRenderComponent(newRender, animation.frames[index]);
		}
	}
}

void drft::system::SyncedAnimationSystem::updateEnd()
{
	for (auto entity : _toRemoveAnimation)
	{
		_registry.remove<SyncedAnimationComponent>(entity);
	}
	_toRemoveAnimation.clear();
}

void drft::system::SyncedAnimationSystem::updateSyncPoints()
{
	for (auto&& [intDec, syncData] : _syncPoints)
	{
		++syncData.elapsed;
		const float numFramesTillNextIndex = TARGET_UPDATES_PER_SECOND / std::abs(convertIntDecToFloat(intDec));
		if (syncData.elapsed >= numFramesTillNextIndex)
		{
			syncData.index++;
			syncData.elapsed -= numFramesTillNextIndex;
		}
	}
}

drft::system::SyncedAnimationSystem::IntDec drft::system::SyncedAnimationSystem::convertFloatToIntDec(float fl) const
{
	int intPortion = static_cast<int>(std::floor(fl));
	int decPortion = static_cast<int>(std::floor((fl - intPortion) * 10));
	return IntDec{ intPortion, decPortion };
}

float drft::system::SyncedAnimationSystem::convertIntDecToFloat(IntDec intDec) const
{
	float result = static_cast<float>(intDec.first);
	result += static_cast<float>(intDec.second) / 10.f;
	return result;
}
