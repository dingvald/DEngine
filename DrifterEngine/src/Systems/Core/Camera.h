#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class Camera : public System
	{
	public:
		using System::System;

		void start() override;
		void updateEnd() override;
		void shutdown() override;

	private:
		entt::entity _camera = entt::null;
	};

}


