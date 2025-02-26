#pragma once
#include "Systems/System.h"
#include "GUI/GUIElement.h"
#include "Events/SendFloatingMessageEvent.h"

namespace drft::system
{
	class HUD : public System
	{
	public:
		using System::System;

		void init() override;
		void update() override;
		void render(sf::RenderTarget& target) override;

	private:
		void createLevelInfo();
		void createHealthBar();
		void createStaminaBar();
		void createInHandsDisplay();
		void createItemsOnGroundDisplay();
		void createHotbar();

		void updateLevelInfo(entt::const_handle player);
		void updateHealthBar(entt::const_handle player);
		void updateStaminaBar(entt::const_handle player);
		void updateItemsOnGround(entt::const_handle player);
		void updateInHandsDisplay(entt::const_handle player);
		void updateFlashEffects();
		void updateHotbar(entt::const_handle player);

		void addItemIcon(gui::Element& container, entt::entity item);

		void queueFlashEffect(sf::Vector2f position, sf::Vector2f size, int ttl, bool fades = false);
		void onHotbarPressed(entt::registry& registry, entt::entity entity);
		void onTakeDamage(entt::registry& registry, entt::entity entity);
		void onConsumeStamina(entt::registry& registry, entt::entity entity);

	private:
		struct FlashEffect
		{
			sf::RectangleShape shape;
			bool fades = false;
			int ttl = 0; // in frames
		};

		sf::Text _lvlText{};
		sf::Text _xpText{};

		sf::RectangleShape _healthBar{};
		sf::RectangleShape _healthBarContainer{};
		sf::Sprite _heartIcon{};

		sf::RectangleShape _staminaBar{};
		sf::RectangleShape _staminaBarContainer{};
		sf::Sprite _staminaIcon{};

		gui::MultiContainer _inHandsDisplay;
		gui::MultiContainer _hotbar;
		gui::List _itemsOnGround{ false };

		std::vector<FlashEffect> _flashEffects;
	};
}



