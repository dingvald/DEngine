#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class Camera : public System
	{
	public:
		void init() override;
		void onStart() override;
		void update(const float dt) override;
		void shutdown() override;

	private:
		entt::entity _camera;
	};

}


