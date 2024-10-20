#include "pch.h"
#include "TweeningSystem.h"
#include <Systems/HelperClasses/InputBuffer.h>
#include <Spatial/Conversions.h>
#include <Utility/Vector3Utils.h>

#include <Components/PositionComponent.h>
#include <Components/TweeningComponent.h>

#pragma optimize("", off)


void drft::system::TweeningSystem::tween(entt::handle entity, Tween tween)
{
	auto& tweening = entity.get_or_emplace<TweeningComponent>();
	tweening.tweens.push_back(std::move(tween));
}

void drft::system::TweeningSystem::onFixedUpdate()
{
	const auto& inputBuffer = _registry.ctx().get<const InputBuffer&>();
	const bool isInputWaiting = !inputBuffer.isEmpty();

	auto view = _registry.view<TweeningComponent>();
	for (auto&& [entity, tweening] : view.each())
	{
		tweening.elapsed++;

		const Tween& tween = tweening.tweens.at(tweening.index);
		const sf::Vector3f start = tweening.index > 0 ? tweening.tweens.at(tweening.index - 1).targetOffset : sf::Vector3f{0, 0, 0};
		const sf::Vector3f delta = tween.targetOffset - start;

		const float progress = tween.easing(tweening.elapsed / tween.time);
		tweening.offset = start + (delta * progress);

		if (tweening.elapsed >= tween.time)
		{
			tween.onFinish({ _registry, entity });
			tweening.index++;
			if (tweening.index >= tweening.tweens.size())
			{
				_registry.remove<TweeningComponent>(entity);
			}
		}
	}
}
