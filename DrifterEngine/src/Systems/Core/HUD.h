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
		void addItemIcon(gui::Element& container, entt::entity item);
	private:
		sf::RectangleShape _healthBar{};
		sf::RectangleShape _healthBarContainer{};
		sf::Sprite _heartIcon{};
		gui::DualContainer _inHandsDisplay;
	};
}



