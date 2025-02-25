#pragma once
#include "Systems/System.h"
#include <Actions/StateActionMap.h>

namespace drft::system
{
	class PlayerInput : public System
	{
	public:
		using System::System;

		virtual void init() override;
		virtual void update() override;

	private:
		using ActionType = std::function<void(entt::handle)>;
		StateActionMap<ActionType> _actionMap;
	};

}


