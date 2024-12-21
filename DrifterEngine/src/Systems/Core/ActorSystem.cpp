#include "pch.h"
#include "ActorSystem.h"
#include "Components/Components.h"
#include "Components/ActorComponent.h"
#include <Components/CurrentActorComponent.h>
#include "Components/DescriptionComponent.h"
#include <Components/TweeningComponent.h>
#include "Components/Tags.h"
#include "Events/GameTickEvent.h"
#include "Events/TurnStartEvent.h"
#include "Events/TurnEndEvent.h"
#include "Utility/EntityHelpers.h"

using namespace entt::literals;

const int AP_PER_TICK = 100;

void drft::system::ActorSystem::init()
{
	_registry.on_destroy<ActorComponent>().connect<&ActorSystem::onActorRemove>(this);
}

void drft::system::ActorSystem::onStart()
{
	_timeKeeper = _registry.create();
	_registry.emplace<ActorComponent>(_timeKeeper, 0, 1.0f, 1.0f);
	_registry.emplace<DescriptionComponent>(_timeKeeper, "Time Keeper", "");

	_managedEntities.insert(_timeKeeper);
	_queue.push_back(_timeKeeper);
	_currentActor = _timeKeeper;
}

void drft::system::ActorSystem::onUpdate(const float)
{
	auto& storage = _registry.storage<CurrentActorComponent>();
	if (storage.size() > 1)
	{
		// This is not good
	}
	else if (storage.size() == 1)
	{
		auto currentActor = storage.begin();
		switch (currentActor->state)
		{
		case CurrentActorState::Pending:
		case CurrentActorState::InProgress:
			return;
		case CurrentActorState::Complete:
			processPoints({ _registry, _currentActor }, currentActor->pointsSpent);
			break;
		default:
			break;
		}
	}

	_registry.clear<CurrentActorComponent>();
	refreshActorQueue();
	_currentActor = rotateQueueToCurrentActor();

	if (_currentActor != _previousActor)
	{
		if (_previousActor != _timeKeeper)
		{
			_dispatcher.trigger(events::TurnEndEvent(_previousActor));
		}
		if (_currentActor != _timeKeeper)
		{
			_dispatcher.trigger(events::TurnStartEvent(_currentActor));
		}
	}

	_previousActor = _currentActor;

	if (_currentActor == _timeKeeper)
	{
		tick();
		return;
	}

	_registry.emplace<CurrentActorComponent>(_currentActor);
}

void drft::system::ActorSystem::shutdown()
{
	_registry.destroy(_timeKeeper);
}

void drft::system::ActorSystem::completeAction(entt::handle entity, ActionCategory category, int cost)
{
	float actionCost = cost;
	if (const auto actorComp = entity.try_get<ActorComponent>())
	{
		switch (category)
		{
		case ActionCategory::Move:
			actionCost *= (1.0f / actorComp->moveSpeed);
			break;
		case ActionCategory::Act:
			actionCost *= (1.0f / actorComp->actSpeed);
			break;
		case ActionCategory::None:
			break;
		}
	}

	if (auto currentActor = entity.try_get<CurrentActorComponent>())
	{
		currentActor->pointsSpent += static_cast<int>(actionCost);
		currentActor->state = CurrentActorState::Complete;
	}
}

void drft::system::ActorSystem::onActorRemove(entt::registry& registry, entt::entity entity)
{
	_managedEntities.erase(entity);
	std::erase(_queue, entity);
}

void drft::system::ActorSystem::processPoints(entt::handle entity, int points) const
{
	if (auto actor = entity.try_get<ActorComponent>())
	{
		actor->ap -= points;
	}
}

void drft::system::ActorSystem::tick()
{
	auto& dispatcher = _registry.ctx().get<entt::dispatcher&>();
	dispatcher.trigger(events::GameTickEvent());

	auto actorView = _registry.view<ActorComponent, component::tag::Active>();
	for (auto&& [entity, actor] : actorView.each())
	{
		if (entity == _timeKeeper) continue;

		actor.ap += AP_PER_TICK;
	}
}

void drft::system::ActorSystem::refreshActorQueue()
{
	_registry.emplace_or_replace<component::tag::Active>(_timeKeeper); // timekeeper should always be active
	auto actorView = _registry.view<ActorComponent, component::tag::Active>();

	// Remove stale entities
	for (auto entity : _managedEntities)
	{
		if (actorView.contains(entity)) continue;

		std::erase(_queue, entity);
		_managedEntities.erase(entity);
	}

	// Add new entities
	for (auto entity : actorView)
	{
		if (_managedEntities.contains(entity)) continue;

		_queue.push_back(entity);
		_managedEntities.insert(entity);
	}
}

entt::entity drft::system::ActorSystem::rotateQueueToCurrentActor()
{
	if (_queue.size() == 0) return _timeKeeper;
	if (_queue.size() == 1) return _queue.front();

	entt::entity result = _queue.front();
	auto pos = _queue.begin();
	for (auto it = _queue.begin(); it != _queue.end(); ++it)
	{
		auto& actor = _registry.get<ActorComponent>(*it);
		if (actor.ap < 0) continue;

		pos = it;
		result = *it;
	}

	std::rotate(_queue.begin(), pos, _queue.end());

	return result;
}