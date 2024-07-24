#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class Camera : public System
	{
	public:
		using System::System;

		void onStart(bool isNewGame) override;
		void onUpdate(const float dt) override;
		void shutdown() override;

	private:
		entt::entity _camera = entt::null;
	};

}


