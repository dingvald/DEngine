#pragma once
#include "Machine.h"

namespace drft::machine
{
	class WildernessHorde : public Machine
	{
		sf::Vector2i getDimensions() const override;
		void layout() override;
	};
}


