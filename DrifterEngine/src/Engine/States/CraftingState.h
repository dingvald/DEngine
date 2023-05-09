#pragma once
#include "State.h"
#include "GUI/GUIElement.h"

namespace drft
{
	class CraftingState : public State
	{
	public:
		CraftingState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;
		void onPush() override;
		void onPop() override;

	private:
		void determineSessionEntities();
		void shutdownSessionEntities();
		void setupCraftingList();
		void refreshCraftingList();

	private:
		entt::entity _sessionEntity = entt::null;
		gui::Panel _craftingBackground;
		gui::Panel _craftingWindow;
		gui::ScrollingList _craftingList{ true };
		gui::ScrollingList _requiresList{ false };
		gui::Stack _popupStack;
		bool _requiresRefresh = false;
	};
}



