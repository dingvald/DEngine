#pragma once
#include "State.h"
#include "GUI/GUIElement.h"

namespace drft
{
	enum class CommandListType
	{
		Inventory,
		Held,
		Worn
	};

	class InventoryState : public State
	{
	private:

	public:
		InventoryState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;
		void onPush() override;
		void onPop() override;

	private:
		void addItemIcon(gui::Element& container, entt::entity item, sf::Vector2f iconSize);
		void setupPanels();
		void setupInventoryDisplay();
		void setupWornItemsDisplay();
		void setupHeldItemsDisplay();

		void updateInventoryDisplay();
		void updateWornItemsDisplay();
		void updateHeldItemsDisplay();
		void determineSessionEntities();
		void shutdownSessionEntities();

		void createItemCommandList(CommandListType type, sf::Vector2f position, unsigned long itemID);
		void tryEquipItem(unsigned long itemID, sf::Vector2f position);
		void tryUnequipItem(unsigned long itemID);

		std::string shortenPartName(const std::string& fullPartName) const;

	private:
		std::vector<entt::entity> _sessionEntities;

		gui::Panel _inventoryBackground;
		gui::FlowControl _flowControl;
		gui::Stack _inventoryStack;
		gui::Blob _inventoryBlob;
	};
}



