#include "pch.h"
#include "TurnManager.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Events/GameTickEvent.h"
#include "Events/TurnStartEvent.h"
#include "Events/TurnEndEvent.h"
#include "Utility/EntityHelpers.h"

void drft::system::TurnManager::init()
{
	registry->on_destroy<component::Actor>().connect<&TurnManager::onActorRemove>(this);
	registry->on_destroy<component::tag::Active>().connect<&TurnManager::onActorRemove>(this);

	_actorQueue = std::make_unique<ActorQueue>(*registry);
}

void drft::system::TurnManager::onStart(bool)
{
	_timeKeeper = registry->create();
	registry->emplace<component::Actor>(_timeKeeper, 0, 1.0f, 1.0f);
	registry->emplace<component::tag::Active>(_timeKeeper);
	registry->emplace<component::Info>(_timeKeeper, "", "Time Keeper", "", "");
	_actorQueue->setSentinel(_timeKeeper);
	_managedEntities.insert(_timeKeeper);
	_currentActor = _timeKeeper;
}

void drft::system::TurnManager::update(const float)
{
	processSpentPoints();
	_actorQueue->refresh(_managedEntities);
	_currentActor = determineCurrentActor();

	if (_currentActor != _previousActor)
	{
		auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
		if (_previousActor != _timeKeeper)
		{
			dispatcher.trigger(events::TurnEndEvent(_previousActor));
		}
		if (_currentActor != _timeKeeper)
		{
			dispatcher.trigger(events::TurnStartEvent(_currentActor));
		}
	}

	_previousActor = _currentActor;
	if (_currentActor == _timeKeeper)
	{
		_actorQueue->tick();
		_actorQueue->rotate();
		_currentActor = _actorQueue->front();
		return;
	}
	registry->emplace_or_replace<component::tag::CurrentActor>(_currentActor);
}

void drft::system::TurnManager::shutdown()
{
	registry->destroy(_timeKeeper);
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

void drft::system::TurnManager::processSpentPoints()
{
	auto spendPointsView = registry->view<component::action::SpendPoints, component::Actor>();
	for (auto [entity, points, actor] : spendPointsView.each())
	{
		actor.ap -= points.amount;
		registry->remove<component::action::SpendPoints>(entity);
	}
}

entt::entity drft::system::TurnManager::determineCurrentActor()
{
	if (_managedEntities.size() == 1) return _actorQueue->front();
	auto currentActor = _actorQueue->front();
	int actorAP = registry->get<component::Actor>(currentActor).ap;
	while (actorAP < 0)
	{
		registry->remove<component::tag::CurrentActor>(currentActor);
		_actorQueue->rotate();
		currentActor = _actorQueue->front();
		actorAP = registry->get<component::Actor>(currentActor).ap;
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

void drft::system::ActorQueue::refresh(std::set<entt::entity>& currentEntities)
{
	auto actorView = registry.view<component::Actor, component::tag::Active>();
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

void drft::system::ActorQueue::sort()
{
	std::stable_sort(_queue.begin(), _queue.end(),
		[this](const entt::entity& a, const entt::entity& b)
		{
			auto& actor_a = this->registry.get<component::Actor>(a);
	auto& actor_b = this->registry.get<component::Actor>(b);

	return actor_a.ap > actor_b.ap;
		}
	);
}

entt::entity drft::system::ActorQueue::front() const
{
	return _queue.front();
}

void drft::system::ActorQueue::tick()
{
	std::cout << "Tick!" << std::endl;
	auto& dispatcher = registry.ctx().get<entt::dispatcher&>();
	dispatcher.trigger(events::GameTickEvent());
	for (auto& e : _queue)
	{
		if (e == _sentinel) continue;
		registry.get<component::Actor>(e).ap += AP_PER_TICK;
	}
}

void drft::system::ActorQueue::printQueue() const
{
	std::cout << "-----QUEUE-----" << std::endl;
	int count = 1;
	for (auto entity : _queue)
	{
		std::cout << count << ". Entity: " << util::getEntityName({ registry, entity }) 
			<< " pts: " << registry.get<component::Actor>(entity).ap << std::endl;
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
