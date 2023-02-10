#include "pch.h"
#include "TurnManager.h"
#include "Events/Events.h"

void drft::system::TurnManager::init()
{
	registry->on_construct<component::Actor>().connect<&TurnManager::onActorAdd>(this);
	registry->on_destroy<component::Actor>().connect<&TurnManager::onActorRemove>(this);
	registry->ctx().get<entt::dispatcher&>().sink<events::SpendActionPoints>().connect<&TurnManager::onSpendActionPoints>(this);

	_timeKeeper = registry->create();
	registry->emplace<component::Actor>(_timeKeeper, 100, 100, 100);

}

void drft::system::TurnManager::update(const float)
{
	auto playerView = registry->view<component::Player, component::Actor>();
	for (auto e : playerView)
	{
		_player = e;
	}

	_currentActor = _queue.front();
	if (_currentActor == _timeKeeper)
	{
		tick();
	}
	registry->emplace_or_replace<component::MyTurn>(_currentActor);

}

void drft::system::TurnManager::onActorAdd(entt::registry& registry, entt::entity entity)
{
	if (registry.any_of<component::Prototype>(entity)) return;
	_queue.push_back(entity);
	sortQueue();
}

void drft::system::TurnManager::onActorRemove(entt::registry& registry, entt::entity entity)
{
	if (registry.any_of<component::Prototype>(entity)) return;
	auto toRemove = std::find(_queue.begin(), _queue.end(), entity);
	if (toRemove != _queue.end())
	{
		_queue.erase(toRemove);
		sortQueue();
	}
}

void drft::system::TurnManager::onSpendActionPoints(events::SpendActionPoints& ev)
{
	auto& actor = registry->get<component::Actor>(_currentActor);
	actor.ap -= ev.amount;
	sortQueue();
	printQueue();
	registry->remove<component::MyTurn>(_currentActor);
}

void drft::system::TurnManager::sortQueue()
{
	std::stable_sort(_queue.begin(), _queue.end(),
		[&](const entt::entity& l, const entt::entity& r) -> bool
		{
			auto& lA = registry->get<component::Actor>(l);
			auto& rA = registry->get<component::Actor>(r);

			return lA.ap > rA.ap;
		}
	);
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
	printQueue();
	for (auto e : _queue)
	{
		if (e == _timeKeeper) continue;
		auto& actor = registry->get<component::Actor>(e);
		actor.ap += AP_PER_TICK;
	}
	sortQueue();
	_currentActor = _queue.front();
	printQueue();
}
