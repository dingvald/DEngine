#include "pch.h"
#include "RealityBubble.h"
#include "Components/ActorComponent.h"
#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "Components/Tags.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Systems/Helpers/GetCurrentCamera.h"

static const float REALITY_RADIUS = 96.0f; // in tiles


void drft::system::RealityBubble::updateBegin(const float dt)
{
	_registry.clear<component::tag::Active>();
	const auto camera = getCurrentCamera(_registry);

	auto actorView = _registry.view<const ActorComponent, const PositionComponent>();
	for (auto&& [entity, actor, pos] : actorView.each())
	{
		const auto distance = spatial::distance3d(camera.position.tile, pos.tile);
		if (distance < REALITY_RADIUS)
		{
			_registry.emplace<component::tag::Active>(entity);
		}
	}
}
