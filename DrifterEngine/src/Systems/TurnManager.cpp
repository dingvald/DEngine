#include "pch.h"
#include "TurnManager.h"
#include "Events/Events.h"

void drft::system::TurnManager::init()
{
	registry->on_destroy<component::Actor>().connect<&TurnManager::onActorRemove>(this);
	registry->on_destroy<component::Active>().connect<&TurnManager::onActorRemove>(this);

	registry->ctx().get<entt::dispatcher&>().sink<events::ActionPerformed>().connect<&TurnManager::onActionPerformed>(this);

	_timeKeeper = registry->create();
	registry->emplace<component::Actor>(_timeKeeper, 100, 1.0f, 1.0f);
	registry->emplace<component::Active>(_timeKeeper);
}

void drft::system::TurnManager::update(const float)
{
	fillActorQueue();

	auto playerView = registry->view<component::Player, component::Actor>
		(entt::exclude<component::Prototype>);

	for (auto e : playerView)
	{
		_player = e;
	}

	_currentActor = _queue.front();
	if (_currentActor == _timeKeeper)
	{
		tick();
		_currentActor = _queue.front();
	}
	auto actor = registry->get<component::Actor>(_currentActor);

	while (actor.ap < 0)
	{
		popFrontPushBack();
		_currentActor = _queue.front();
		actor = registry->get<component::Actor>(_currentActor);
	}

	registry->emplace_or_replace<component::MyTurn>(_currentActor);

}

void drft::system::TurnManager::onActorRemove(entt::registry& registry, entt::entity entity)
{
	if (registry.any_of<component::Prototype>(entity)) return;
	if (_managedEntities.contains(entity))
	{
		_managedEntities.erase(entity);
	}
	auto toRemove = std::find(_queue.begin(), _queue.end(), entity);
	if (toRemove != _queue.end())
	{
		_queue.erase(toRemove);
	}
}

void drft::system::TurnManager::onActionPerformed(events::ActionPerformed& ev)
{
	auto& actor = registry->get<component::Actor>(_currentActor);
	actor.ap -= ev.amount;
	_animationRequired = ev.hasAnimation;
}

void drft::system::TurnManager::fillActorQueue()
{
	auto actorView = registry->view<component::Actor, component::Active>(entt::exclude<component::Prototype>);
	for (auto entity : actorView)
	{
		if (_managedEntities.contains(entity)) continue;

		_queue.push_back(entity);
		_managedEntities.insert(entity);
	}
}

void drft::system::TurnManager::popFrontPushBack()
{
	auto front = _queue.front();
	_queue.erase(_queue.begin());
	_queue.push_back(front);
}

void drft::system::TurnManager::printQueue()
{
	std::cout << "Turn Queue:" << std::endl;
	int count = 1;
	for (auto e : _queue)
	{
		if (e == _timeKeeper)
		{
			auto& tk = registry->get<component::Actor>(_timeKeeper);
			std::cout << count++ << ". " << "Time Keeper" << ": " << tk.ap << " pts" << std::endl;
			continue;
		}
		auto& info = registry->get<component::Info>(e);
		auto& actor = registry->get<component::Actor>(e);
		std::cout << count << ". " << info.name << ": " << actor.ap << " pts" << std::endl;
		++count;
	}
	std::cout << std::endl;
}

void drft::system::TurnManager::tick()
{
	std::cout << "Tick!" << std::endl;
	for (auto e : _queue)
	{
		if (e == _timeKeeper) continue;
		auto& actor = registry->get<component::Actor>(e);
		actor.ap += AP_PER_TICK;
	}
	popFrontPushBack();
}

