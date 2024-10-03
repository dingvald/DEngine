#pragma once
#include "Systems/System.h"
#include "Systems/Helpers/ItemDatabase.h"

namespace drft::system
{
	class ItemUniqueIDGenerator :public System
	{
	public:
		using System::System;

		void init() override;
		void onStart() override;

	private:
		void onItemAdd(entt::registry& registry, entt::entity entity);
		void onItemIDTrackerAdded(entt::registry& registry, entt::entity entity);

	private:
		friend class ItemDatabase;
		entt::entity _idTracker = entt::null;
		static inline std::unordered_map<unsigned long, entt::entity> _itemToEntityCache;
	};
}



