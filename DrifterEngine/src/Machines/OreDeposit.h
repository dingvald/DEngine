#pragma once
#include "Machine.h"

namespace drft::machine
{
	class OreDeposit : public Machine
	{
		sf::Vector2i getDimensions() const override;
		void layout() override;
	};
}

