#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class ItemUniqueIDGenerator :public System
	{
	public:
		void init() override;

		void save(cereal::JSONOutputArchive& oarchive) override;
		void load(cereal::JSONInputArchive& iarchive) override;

	private:
		void onItemAdd(entt::registry& registry, entt::entity);

	private:
		unsigned long _nextAvailableID = 1;
	};
}



