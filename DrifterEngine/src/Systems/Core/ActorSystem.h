#pragma once
#include "Systems/System.h"


namespace drft::system
{
	class ActorSystem : public System
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
		void processPoints(entt::handle entity, int points) const;
		void tick();

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


