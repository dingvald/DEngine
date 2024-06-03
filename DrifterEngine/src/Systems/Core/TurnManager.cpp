#include "pch.h"
#include "TurnManager.h"
#include "Components/Components.h"
#include "Components/ActorComponent.h"
#include "Components/DescriptionComponent.h"
#include "Components/Tags.h"
#include "Events/GameTickEvent.h"
#include "Events/TurnStartEvent.h"
#include "Events/TurnEndEvent.h"
#include "Utility/EntityHelpers.h"

void drft::system::TurnManager::init()
{
	_registry->on_construct<component::action::SpendPoints>().connect<&TurnManager::onSpendActionPoints>(this);
	_registry->on_destroy<ActorComponent>().connect<&TurnManager::onActorRemove>(this);

	_actorQueue = std::make_unique<ActorQueue>(*_registry);
}

void drft::system::TurnManager::onStart(bool)
{
	_timeKeeper = _registry->create();
	_registry->emplace<ActorComponent>(_timeKeeper, 0, 1.0f, 1.0f);
	_registry->emplace<DescriptionComponent>(_timeKeeper, "Time Keeper", "");

	_actorQueue->setSentinel(_timeKeeper);
	_managedEntities.insert(_timeKeeper);
	_currentActor = _timeKeeper;
}

void drft::system::TurnManager::update(const float)
{
	_actorQueue->refresh(_managedEntities);
	_currentActor = determineCurrentActor();

	if (_currentActor != _previousActor)
	{
		if (_previousActor != _timeKeeper)
		{
			_dispatcher->trigger(events::TurnEndEvent(_previousActor));
		}
		if (_currentActor != _timeKeeper)
		{
			_dispatcher->trigger(events::TurnStartEvent(_currentActor));
		}
	}

	_registry->clear<component::tag::CurrentActor>();
	_previousActor = _currentActor;
	if (_currentActor == _timeKeeper)
	{
		_actorQueue->tick();
		_actorQueue->rotate();
		_currentActor = _actorQueue->front();
		return;
	}

	_registry->emplace_or_replace<component::tag::CurrentActor>(_currentActor);
}

void drft::system::TurnManager::onUpdateEnd()
{
	_registry->clear<component::action::SpendPoints>();
}

void drft::system::TurnManager::shutdown()
{
	_registry->destroy(_timeKeeper);
}

void drft::system::TurnManager::onActorRemove(entt::registry& registry, entt::entity entity)
{
	if (_managedEntities.contains(entity))
	{
		_managedEntities.erase(entity);
	}
	registry.remove<component::tag::CurrentActor>(entity);
	_actorQueue->remove(entity);
}

void drft::system::TurnManager::onSpendActionPoints(entt::registry& registry, entt::entity entity)
{
	if (auto actor = registry.try_get<ActorComponent>(entity))
	{
		auto& spentPoints = registry.get<component::action::SpendPoints>(entity);
		actor->ap -= spentPoints.amount;
	}
}

entt::entity drft::system::TurnManager::determineCurrentActor()
{
	if (_managedEntities.size() == 1) return _actorQueue->front();
	auto currentActor = _actorQueue->front();
	int actorAP = _registry->get<ActorComponent>(currentActor).ap;
	while (actorAP < 0)
	{
		_actorQueue->rotate();
		currentActor = _actorQueue->front();
		actorAP = _registry->get<ActorComponent>(currentActor).ap;
	}

	return currentActor;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Actor Queue
///////////////////////////////////////////////////////////////////////////////////////////////////

drft::system::ActorQueue::ActorQueue(entt::registry& registry)
	: registry(registry)
	, _sentinel(entt::null)
{}

void drft::system::ActorQueue::refresh(std::unordered_set<entt::entity>& currentEntities)
{
	registry.emplace_or_replace<component::tag::Active>(_sentinel); // Sentinel should always be active
	auto actorView = registry.view<ActorComponent, component::tag::Active>();
	for (auto entity : currentEntities)
	{
		if (actorView.contains(entity)) continue;

		remove(entity);
		currentEntities.erase(entity);
	}
	for (auto entity : actorView)
	{
		if (currentEntities.contains(entity)) continue;

		_queue.push_front(entity);
		currentEntities.insert(entity);
	}
}

void drft::system::ActorQueue::rotate()
{
	auto front = _queue.front();
	_queue.pop_front();
	_queue.push_back(front);
}

entt::entity drft::system::ActorQueue::front() const
{
	return _queue.front();
}

void drft::system::ActorQueue::tick()
{
	auto& dispatcher = registry.ctx().get<entt::dispatcher&>();
	dispatcher.trigger(events::GameTickEvent());
	for (auto& e : _queue)
	{
		if (e == _sentinel) continue;
		registry.get<ActorComponent>(e).ap += AP_PER_TICK;
	}
}

void drft::system::ActorQueue::printQueue() const
{
	std::cout << "-----QUEUE-----" << std::endl;
	int count = 1;
	for (auto entity : _queue)
	{
		std::cout << count << ". Entity: " << util::getEntityName({ registry, entity }) 
			<< " pts: " << registry.get<ActorComponent>(entity).ap << std::endl;
		++count;
	}
	std::cout << "---END QUEUE---" << std::endl;
}

void drft::system::ActorQueue::setSentinel(entt::entity sentinel)
{
	_sentinel = sentinel;
	_queue.push_back(_sentinel);

}

void drft::system::ActorQueue::remove(entt::entity entity)
{
	auto toRemove = std::find(_queue.begin(), _queue.end(), entity);
	if (toRemove != _queue.end())
	{
		_queue.erase(toRemove);
	}
}
