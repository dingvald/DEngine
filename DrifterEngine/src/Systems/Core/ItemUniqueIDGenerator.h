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

		void save(cereal::JSONOutputArchive& oarchive) override;
		void load(cereal::JSONInputArchive& iarchive) override;

	private:
		void onItemAdd(entt::registry& registry, entt::entity);

	private:
		friend class ItemDatabase;
		unsigned long _nextAvailableID = 1;
		static std::unordered_map<unsigned long, entt::entity> _itemToEntityCache;
	};
}



