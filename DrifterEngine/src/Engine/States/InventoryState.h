#pragma once
#include "State.h"
#include "GUI/GUIElement.h"

namespace drft
{
	enum class SessionType
	{
		Equip,
		Inventory
	};

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
		SessionType _sessionType = SessionType::Inventory;
		unsigned long _currentItemID = 0; // Ugly hackz - should try to minimize state at this scope

		gui::Panel _inventoryBackground;
		gui::FlowControl _flowControl;
		gui::Stack _inventoryStack;
		gui::Blob _inventoryBlob;
	};
}



