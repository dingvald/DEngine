#pragma once
#include <Systems/System.h>

#include <AI/Utility/UtilityAI.h>
#include <AI/InputClearingHouse/AIDataClearingHouse.h>
#include <AI/AiActions/AiActionRegistry.h>

namespace drft::system
{
	class AISystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void update() override;

	private:
		UtilityAI<entt::entity> _utility;

		AIDataClearingHouse _inputClearingHouse{_registry};
		AiActionRegistry _actionRegistry;
	};
}