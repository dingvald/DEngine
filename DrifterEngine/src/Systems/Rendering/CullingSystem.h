#pragma once
#include "Systems/System.h"


namespace drft::system
{
	class CullingSystem : public System
	{
	public:
		using System::System;

		void onFixedUpdate() override;
		void onFixedUpdateEnd() override;

	private:
		sf::FloatRect addBufferToViewport(const sf::FloatRect& viewport) const;
	};
}


