#include "pch.h"
#include "RealityBubble.h"
#include "Components/ActorComponent.h"
#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"
#include "Components/Tags.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Services/DebugInfo.h"
#include "Systems/Helpers/GetCurrentCamera.h"

static const int REALITY_RADIUS = 96; // in tiles

void drft::system::RealityBubble::init()
{
}

void drft::system::RealityBubble::update(const float)
{
	const auto camera = getCurrentCamera(*_registry);

	auto actorView = _registry->view<const ActorComponent, const PositionComponent>();
	int activeActors = 0;
	for (auto&& [entity, actor, pos] : actorView.each())
	{
		const auto distance = spatial::distance(camera.position, pos.position);
		if (distance < REALITY_RADIUS)
		{
			_registry->emplace<component::tag::Active>(entity);
			activeActors++;
		}
	}

	service::DebugInfo::instance().putInfo("Actors Active", std::to_string(activeActors));
}

void drft::system::RealityBubble::onUpdateEnd()
{
	_registry->clear<component::tag::Active>();
}
