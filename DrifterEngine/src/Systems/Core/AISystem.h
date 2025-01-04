#pragma once
#include <Systems/System.h>

#include <AI/Utility/UtilityAI.h>

class AIDataClearingHouse;
class AiActionRegistry;

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

		std::unique_ptr<AIDataClearingHouse> _inputClearingHouse;
		std::unique_ptr<AiActionRegistry> _actionRegistry;
	};
}