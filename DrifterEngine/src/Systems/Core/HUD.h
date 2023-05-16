#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class HUD : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;
		void render(sf::RenderTarget& target) override;

	private:
		sf::RectangleShape _healthBar{};
		sf::RectangleShape _healthBarContainer{};
		sf::Sprite _heartIcon{};
	};
}



