#pragma once
#include <States/State.h>
#include <Components/Wrappers/ContainerWrapper.h>
#include "Components/BodyComponent.h"

namespace drft
{
	class InventoryState : public State
	{
	private:
		struct DraggingFromInventoryContext
		{
			entt::const_handle item;
			ContainerWrapper* container;
			size_t index;
		};
		struct DraggingFromEquipmentContext
		{
			entt::const_handle item;
			unsigned int itemID;
			BodyComponent* body;
			std::string slot;
		};

		using DraggingContext = std::variant<std::monostate, DraggingFromInventoryContext, DraggingFromEquipmentContext>;

		struct DraggingItem
		{
			DraggingItem() = default;
			DraggingItem(DraggingContext ctx, tgui::Group::Ptr gui);
			~DraggingItem();

			entt::const_handle getItem() const;
			void undo();
			void setClickHandled();
			bool isClickHandled() const;

			tgui::Group::Ptr display;
			DraggingContext context;

		private:
			bool _isClickHandled = false;
		};

	public:
		InventoryState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update() override;

	private:

		void determineSessionEntities();

		void setupInventoryEntryTemplate(tgui::Panel::Ptr templatePanel);
		void setupPaperdollNodeTemplate();

		void refreshInventoryUI(tgui::PanelListBox::Ptr list, bool shouldSort);
		void refreshEquipmentUI(tgui::Grid::Ptr equipment);

		void addItemToInventoryUI(size_t index, entt::const_handle item, tgui::Panel::Ptr panel);
		void addItemToEquipmentUI(const std::string& slotName, const BodyPart::Slot& slot, tgui::Panel::Ptr panel);

		std::string shortenPartName(const std::string& fullPartName) const;

		void onLeftMousePressInventoryItem(size_t index, entt::const_handle item);
		void onLeftMousePressEquipmentItem(entt::const_handle item);
		void onLeftMousePressInventoryWindow(tgui::Vector2f position);

	private:
		ContainerWrapper _container;
		entt::handle _sessionEntity;
		tgui::Panel::Ptr _paperdollNodeTemplate;
		std::optional<DraggingItem> _draggingItem;
	};
}



