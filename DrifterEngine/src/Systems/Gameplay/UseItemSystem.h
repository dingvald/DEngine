#pragma once
#include "Systems/System.h"
#include "UseItemFunctions/UseItemFunctions.h"


namespace drft::system
{
	using UseFunc = std::function<void(entt::registry&, entt::entity, use::Parameters)>;
	const std::unordered_map<std::string, UseFunc> String2UseFunc =
	{
		{"Heal", use::heal}
	};

	class UseItemSystem : public System
	{
	public:
		void init() override;
		void update(const float dt) override;

	private:
		void onUseItem(entt::registry& registry, entt::entity entity);
	};
}


