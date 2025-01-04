#pragma once
#include "IAction.h"
#include "Actions/AiActionTypes.h"

namespace drft::goap
{
	class ActionRegistry
	{
	public:
		static void bind();
		static const IAction& get(AiAction actionType);

	private:
		using ActionMap = std::unordered_map<AiAction, std::unique_ptr<IAction>>;
		using ActionFactories = std::unordered_map<AiAction, std::function<std::unique_ptr<IAction>()>>;
		static ActionMap _actions;
		static ActionFactories _factories;
	};
}

