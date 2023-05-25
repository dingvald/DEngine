#pragma once
#include "Systems/System.h"
#include "GUI/GUIElement.h"

namespace drft::system
{
	class HUD : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;
		void render(sf::RenderTarget& target) override;

	private:
		void createHealthBar();
		void createStaminaBar();
		void createInHandsDisplay();
		void addItemIcon(gui::Element& container, entt::entity item);

	private:
		sf::RectangleShape _healthBar{};
		sf::RectangleShape _healthBarContainer{};
		sf::Sprite _heartIcon{};

		sf::RectangleShape _staminaBar{};
		sf::RectangleShape _staminaBarContainer{};
		sf::Sprite _staminaIcon{};

		gui::DualContainer _inHandsDisplay;
	};
}



