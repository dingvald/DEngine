#pragma once
#include "Systems/System.h"


namespace drft::system
{
	enum class ActionCategory
	{
		None,
		Move,
		Act,
	};

	class ActorSystem : public System
	{
	public:
		static const int BASE_ACTION_COST = 100;
	public:
		using System::System;

		static void completeAction(entt::handle entity, ActionCategory category, int cost = BASE_ACTION_COST);

	private:
		void init() override;
		void start() override;
		void update() override;
		void shutdown() override;

		void onActorRemove(entt::registry& registry, entt::entity entity);
		void processPoints(entt::handle entity, int points) const;
		void tick();
		void rotateQueue();

		void refreshActorQueue();
		entt::entity rotateQueueToCurrentActor();

	private:
		entt::entity _timeKeeper = entt::null;
		entt::entity _currentActor = entt::null;
		entt::entity _previousActor = entt::null;

		std::vector<entt::entity> _queue;
		std::unordered_set<entt::entity> _managedEntities;
	};

	
}


