#pragma once
#include "Systems/System.h"


namespace drft::system
{
	class CullingSystem : public System
	{
		using System::System;

		void render(sf::RenderTarget& target) override;
	};
}


