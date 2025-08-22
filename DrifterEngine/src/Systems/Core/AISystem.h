#pragma once
#include <Systems/System.h>

#include <AI/Utility/UtilityAI.h>
#include <AI/InputClearingHouse/AIDataClearingHouse.h>
#include <AI/AiActions/AiActionRegistry.h>
#include <AI/Sensor/SensorRunner.h>

struct UtilityAIComponent;

namespace drft::system
{
	class AiSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void update() override;

		void setDefaultAction(const IAiAction* defaultAction);
		void setMoveToAction(const IAiAction* moveToAction);
		std::pair<const IAiAction*, entt::entity> selectAction(const UtilityAI<entt::entity>::ScoredActions& actions, entt::const_handle actor) const;
		void selectAndPerformAction(entt::const_handle actor, const UtilityAIComponent& ai) const;

	private:
		UtilityAI<entt::entity> _utility;
		SensorRunner _sensors;

		AIDataClearingHouse _inputClearingHouse{_registry};
		AiActionRegistry _actionRegistry;
		const IAiAction* _moveToAction = nullptr;
		const IAiAction* _defaultAction = nullptr;
	};
}