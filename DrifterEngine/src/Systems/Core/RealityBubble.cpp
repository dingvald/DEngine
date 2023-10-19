#include "pch.h"
#include "RealityBubble.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Services/DebugInfo.h"

static const int REALITY_RADIUS = 96; // in tiles

void drft::system::RealityBubble::init()
{
	registry->on_construct<component::Actor>().connect<&RealityBubble::onActorAddOrUpdate>(this);
	registry->on_update<component::Actor>().connect<&RealityBubble::onActorAddOrUpdate>(this);
	registry->on_destroy<component::Actor>().connect<&RealityBubble::onActorRemove>(this);

	registry->on_construct<component::tag::Active>().connect<&RealityBubble::onActiveAdd>(this);
	registry->on_destroy<component::tag::Active>().connect<&RealityBubble::onActiveRemove>(this);
}

void drft::system::RealityBubble::update(const float)
{
	auto cameraView = registry->view<const component::Camera, const component::Position>();
	for (auto&& [entity, camera, position] : cameraView.each())
	{
		_cameraPosition = position.position;
	}

	auto actorView = registry->view<const component::Actor, const component::Position>();
	for (auto&& [entity, actor, pos] : actorView.each())
	{
		const auto distance = spatial::distance(_cameraPosition, pos.position);

		if (distance > REALITY_RADIUS)
		{
			registry->remove<component::tag::Active>(entity);
		}
		else
		{
			registry->emplace_or_replace<component::tag::Active>(entity);
		}
	}

	service::DebugInfo::instance().putInfo("Actors Active", std::to_string(_activeActors));
}

void drft::system::RealityBubble::onActorAddOrUpdate(entt::registry& registry, entt::entity entity)
{
	if (!registry.any_of<component::Position>(entity)) return;

	const auto pos = registry.get<component::Position>(entity).position;
	const auto distance = spatial::distance(_cameraPosition, pos);

	if (distance > REALITY_RADIUS)
	{
		registry.remove<component::tag::Active>(entity);
	}
	else
	{
		registry.emplace_or_replace<component::tag::Active>(entity);
	}
}

void drft::system::RealityBubble::onActorRemove(entt::registry& registry, entt::entity entity)
{
	registry.remove<component::tag::Active>(entity);
}

void drft::system::RealityBubble::onActiveAdd(entt::registry& registry, entt::entity entity)
{
	++_activeActors;
}

void drft::system::RealityBubble::onActiveRemove(entt::registry& registry, entt::entity entity)
{
	--_activeActors;
}
