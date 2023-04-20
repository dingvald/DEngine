#pragma once
#include "Systems/System.h"


namespace component
{
	struct Actor;
}

namespace drft::system
{
	const int AP_PER_TICK = 100;

	class ActorQueue
	{
	public:
		ActorQueue(entt::registry& registry);
		void refresh(std::set<entt::entity>& currentEntities);
		void rotate();
		void sort();
		entt::entity front() const;
		void tick();
		void printQueue() const;
		void setSentinel(entt::entity sentinel);
		void remove(entt::entity);
	private:
		entt::registry& registry;
		entt::entity _sentinel;
		std::deque<entt::entity> _queue;
	};

	class TurnManager : public System
	{
	public:
		virtual void init() override;
		virtual void update(const float dt) override;

	private:
		void onActorRemove(entt::registry& registry, entt::entity entity);
		void processSpentPoints();
		entt::entity determineCurrentActor();

	private:
		std::unique_ptr<ActorQueue> _actorQueue;
		std::set<entt::entity> _managedEntities;
		entt::entity _timeKeeper;

		entt::entity _currentActor = entt::null;
		entt::entity _previousActor = entt::null;
	};

	
}


