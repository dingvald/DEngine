#pragma once
#include "Systems/System.h"
#include <GUI/AbilityTooltip.h>

namespace drft::events
{
	struct ChangeHUDEnabledEvent;
}

namespace Internal
{
	struct HudRefreshRequestEvent;
}

namespace drft::system
{
	class HUD : public System
	{
	public:
		using System::System;

		static void setVisible(entt::registry& registry, bool shouldBeVisible);
		static void setEnabled(entt::registry& registry, bool shouldBeEnabled);
		static void refresh(entt::registry& registry);

	private:
		void init() override;
		void start() override;
		void update() override;
		void shutdown() override;

		void createLevelInfo();
		void createHealthBar();
		void createStaminaBar();
		void createInHandsDisplay();
		void createItemsOnGroundDisplay();
		void createHotbar(entt::const_handle player);

		void updateLevelInfo(entt::const_handle player);
		void updateHealthBar(entt::const_handle player);
		void updateStaminaBar(entt::const_handle player);
		void updateItemsOnGround(entt::const_handle player);
		void updateInHandsDisplay(entt::const_handle player);
		void updateFlashEffects();
		void updateHotbar(tgui::GrowHorizontalLayout::Ptr hotbar, entt::const_handle player);
		void refreshHotbar(tgui::GrowHorizontalLayout::Ptr hotbar, entt::const_handle player);

		void queueFlashEffect(sf::Vector2f position, sf::Vector2f size, int ttl, bool fades = false);
		void onHotbarPressed(entt::registry& registry, entt::entity entity);
		void onTakeDamage(entt::registry& registry, entt::entity entity);
		void onConsumeStamina(entt::registry& registry, entt::entity entity);
		void onHotbarComponentUpdated(entt::registry& registry, entt::entity entity);

		void onChangeHUDEnabledEvent(const events::ChangeHUDEnabledEvent& ev);
		void onHudRefreshRequestEvent(const Internal::HudRefreshRequestEvent& ev);

		void onEnterAbilityContainingWidget(const drft::IAbility& ability, entt::const_handle entity);
		void onExitAbilityContainingWidget();

	private:
		struct FlashEffect
		{
			tgui::Panel::Ptr shape;
			bool fades = false;
			int ttl = 0; // in frames
		};

		std::vector<FlashEffect> _flashEffects;
		tgui::Group::Ptr _gui;
		tgui::Group::Ptr _templateHotbarIcon;
		std::optional<AbilityTooltip> _abilityTooltip;
	};
}



