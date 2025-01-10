#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class Camera : public System
	{
	public:
		using System::System;

		

	private:
		void start() override;
		void render(sf::RenderTarget& target) override;
		void shutdown() override;

		entt::entity tryFindTarget() const;

	private:
		entt::entity _camera = entt::null;
	};

}


