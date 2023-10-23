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

	struct SessionContext
	{
	public:
		void setCurrentItem(unsigned long itemID) { _currentItemID = itemID; }
		unsigned long getCurrentItem() const { return _currentItemID; }
		void setCurrentSlot(std::string slotname) { _currentSlotName = slotname; }
		const std::string& getCurrentSlot() const { return _currentSlotName; }

		void reset()
		{
			_currentItemID = 0;
			_currentSlotName = "None";
		}

	private:
		unsigned long _currentItemID = 0;
		std::string _currentSlotName = "None";
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

		void tryEquipItem(unsigned long itemID);

	private:
		std::vector<entt::entity> _sessionEntities;
		SessionContext _sessionContext;

		gui::Panel _inventoryBackground;
		gui::FlowControl _flowControl;
		gui::Stack _inventoryStack;
		gui::Blob _inventoryBlob;
	};
}



