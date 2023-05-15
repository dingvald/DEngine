#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class RenderingCleanup : public System
	{
	public:
		void init() override;
		void render(sf::RenderTarget& target) override;
	};
}

