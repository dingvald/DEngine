#pragma once
#include "State.h"
#include "GUI/GUIElement.h"

namespace drft
{
	class InventoryState : public State
	{
	private:

	public:
		InventoryState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev);
		bool update(const float dt);
		void render(sf::RenderTarget& target);
		void onPush() override;
		void onPop() override;

	private:
		void setupPanels();
		void setupInventoryGrid();
		void setupEquipmentGrid();
		void determineSessionEntities();
		void shutdownSessionEntities();

	private:
		std::vector<entt::entity> _sessionEntities;
		gui::Panel _inventoryBackground;
		gui::Stack _inventoryStack;
		gui::Blob _inventoryBlob;
	};
}



