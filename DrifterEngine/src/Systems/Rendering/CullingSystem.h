#pragma once
#include "Systems/System.h"


namespace drft::system
{
	class CullingSystem : public System
	{
	public:
		using System::System;

		void update() override;

	private:
		sf::FloatRect addBufferToViewport(const sf::FloatRect& viewport) const;
	};
}


