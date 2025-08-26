#include "pch.h"
#include "VisualActorSensor.h"

#include <AI/AiTargetTypes.h>
#include <AI/Blackboard/Blackboard.h>

#include "Components/PositionComponent.h"
#include <Components/ActorComponent.h>
#include <Components/VisionComponent.h>
#include <Components/LightBlockingComponent.h>
#include <Components/Tags.h>

#include <Spatial/WorldGrid.h>
#include <Spatial/Helpers.h>

using namespace entt::literals;

void VisualActorSensor::sense(entt::const_handle agent, Blackboard& blackboard) const
{
	auto& grid = agent.registry()->ctx().get<drft::spatial::WorldGrid>();

	auto& vision = agent.get<VisionComponent>();
	auto& agentPosition = agent.get<PositionComponent>();

	auto actorView = agent.registry()->view<ActorComponent, PositionComponent, component::tag::Active>();
	for (auto&& [entity, actor, position] : actorView.each())
	{
		if (entity == agent.entity()) continue;

		if (drft::spatial::distance3d(agentPosition.tile, position.tile) > vision.sightRange) continue;

		auto entities = grid.castRay(agentPosition.tile, position.tile,
			[registry = agent.registry()](entt::entity entity) -> bool {
				return registry->all_of<LightBlockingComponent>(entity);
			});

		if (entities.empty())
		{
			blackboard.pushToList(AiTargetTypes::toIdHash(AiTargetType::Actors), entity);
		}
	}
}

bool VisualActorSensor::canUseSensor(entt::const_handle agent)
{
	return agent.all_of<VisionComponent>();
}
