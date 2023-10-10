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
		void updateFloatingMessagesDisplay(entt::const_handle player);
		void updateFlashEffects();
		void addItemIcon(gui::Element& container, entt::entity item);

		void queueFloatingMessage(const std::string& message, sf::Color color, sf::Vector2i position, sf::Vector2f velocity, int ttl, bool isScreenSpace);
		void queueFlashEffect(sf::Vector2f position, sf::Vector2f size, int ttl);

		void onSendFloatingMessageEvent(events::SendFloatingMessageEvent& ev);
		void onItemBreakEvent(events::ItemBreakEvent& ev);
		void onTakeDamage(entt::registry& registry, entt::entity entity);
		void onConsumeStamina(entt::registry& registry, entt::entity entity);
		void onLevelUp(entt::registry& registry, entt::entity entity);

	private:
		struct FlashEffect
		{
			sf::RectangleShape shape;
			int ttl = 0; // in frames
		};
		struct FloatingMessage
		{
			sf::Text text;
			sf::Vector2i position;
			sf::Vector2f velocity;
			int ttl = 0;
			bool isScreenSpace = false;
			sf::Vector2f distanceTraveled;
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

		// Messages
		std::vector<FloatingMessage> _floatingMessages;
		std::vector<FlashEffect> _flashEffects;
	};
}



