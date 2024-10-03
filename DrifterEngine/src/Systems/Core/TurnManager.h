#pragma once
#include "Systems/System.h"


namespace drft::system
{
	const int AP_PER_TICK = 100;

	class ActorQueue
	{
	public:
		ActorQueue(entt::registry& registry);
		void refresh(std::unordered_set<entt::entity>& currentEntities);
		void rotate();
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
		using System::System;

		void init() override;
		void onStart() override;
		void onUpdate(const float dt) override;
		void onUpdateEnd() override;
		void shutdown() override;

	private:
		void onActorRemove(entt::registry& registry, entt::entity entity);
		void onSpendActionPoints(entt::registry& registry, entt::entity entity);

		entt::entity determineCurrentActor();

	private:
		std::unique_ptr<ActorQueue> _actorQueue;
		std::unordered_set<entt::entity> _managedEntities;
		entt::entity _timeKeeper = entt::null;
		entt::entity _currentActor = entt::null;
		entt::entity _previousActor = entt::null;
	};

	
}


