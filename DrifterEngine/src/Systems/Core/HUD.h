#pragma once
#include "Systems/System.h"
#include "GUI/GUIElement.h"
#include "Events/ItemBreakEvent.h"
#include "Events/SendFloatingMessageEvent.h"

namespace drft::system
{
	class HUD : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;
		void render(sf::RenderTarget& target) override;

	private:
		void createLevelInfo();
		void createHealthBar();
		void createStaminaBar();
		void createInHandsDisplay();
		void createItemsOnGroundDisplay();

		void updateLevelInfo(entt::const_handle player);
		void updateHealthBar(entt::const_handle player);
		void updateStaminaBar(entt::const_handle player);
		void updateItemsOnGround(entt::const_handle player);
		void updateInHandsDisplay(entt::const_handle player);
		void updateFlashEffects();
		void addItemIcon(gui::Element& container, entt::entity item);

		void queueFlashEffect(sf::Vector2f position, sf::Vector2f size, int ttl);
		void onTakeDamage(entt::registry& registry, entt::entity entity);
		void onConsumeStamina(entt::registry& registry, entt::entity entity);

	private:
		struct FlashEffect
		{
			sf::RectangleShape shape;
			int ttl = 0; // in frames
		};

		// Static HUD
		sf::Text _lvlText{};
		sf::Text _xpText{};

		sf::RectangleShape _healthBar{};
		sf::RectangleShape _healthBarContainer{};
		sf::Sprite _heartIcon{};

		sf::RectangleShape _staminaBar{};
		sf::RectangleShape _staminaBarContainer{};
		sf::Sprite _staminaIcon{};

		gui::MultiContainer _inHandsDisplay;
		gui::List _itemsOnGround{ false };

		std::vector<FlashEffect> _flashEffects;
	};
}



