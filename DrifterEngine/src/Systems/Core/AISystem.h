#pragma once
#include <Systems/System.h>

#include <AI/Utility/UtilityAI.h>
#include <AI/InputClearingHouse/AIDataClearingHouse.h>
#include <AI/AiActions/AiActionRegistry.h>

namespace drft::system
{
	class AiSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void update() override;

		std::pair<const IAiAction*, entt::entity> selectAction(const UtilityAI<entt::entity>::ScoredActions& actions, entt::const_handle actor) const;

	private:
		UtilityAI<entt::entity> _utility;

		AIDataClearingHouse _inputClearingHouse{_registry};
		AiActionRegistry _actionRegistry;
	};
}