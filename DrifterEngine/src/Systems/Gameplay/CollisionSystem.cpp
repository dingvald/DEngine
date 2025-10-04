#include "pch.h"
#include "CollisionSystem.h"

#include "Components/Actions/MeleeAttackAction.h"
#include "Components/Actions/WaitAction.h"
#include "Components/CollisionComponent.h"

#include "Systems/Gameplay/FactionSystem.h"

void drft::system::CollisionSystem::init()
{
	_registry.on_construct<CollisionComponent>().connect<&CollisionSystem::onCollisionAdded>(this);
}

void drft::system::CollisionSystem::updateEnd(const float dt)
{
	_registry.clear<CollisionComponent>();
}

void drft::system::CollisionSystem::onCollisionAdded(entt::registry& registry, entt::entity entity) const
{
	const auto& collisionComponent = registry.get<CollisionComponent>(entity);
	entt::const_handle handle = { registry, entity };

	switch (determineTargetRelationship(handle, collisionComponent.blockers))
	{
	case drft::system::Relationship::Friendly:
		// TODO: Implement swap
		_registry.emplace_or_replace<WaitAction>(entity);
		break;
	case drft::system::Relationship::Neutral:
	case drft::system::Relationship::Hostile:
		_registry.emplace_or_replace<MeleeAttackAction>(entity, collisionComponent.direction);
		break;
	default:
		break;
	}
}

drft::system::Relationship drft::system::CollisionSystem::determineTargetRelationship(entt::const_handle sourceEntity, const std::vector<entt::entity>& entities) const
{
	Relationship targetRelationship = Relationship::Neutral;
	for (auto&& blocker : entities)
	{
		targetRelationship = FactionSystem::resolveRelationship(sourceEntity, { *sourceEntity.registry(), blocker});
		// if there are any hostiles, then all targets at that cell are considered hostile
		if (targetRelationship == Relationship::Hostile)
		{
			return targetRelationship;
		}
	}
	return targetRelationship;
}
