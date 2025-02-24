#pragma once
#include <States/State.h>
#include <Systems/HelperClasses/ContainerWrapper.h>

struct BodyPart;

namespace drft
{
	class InventoryState : public State
	{
	private:

	public:
		InventoryState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;

	private:
		void determineSessionEntities();

		void setupInventoryEntryTemplate(tgui::Panel::Ptr templatePanel);
		void setupEquipmentEntryTemplate(tgui::Panel::Ptr templatePanel);

		void refreshInventoryUI(tgui::PanelListBox::Ptr list);
		void refreshEquipmentUI(tgui::PanelListBox::Ptr equipment);

		void addItemToInventoryUI(entt::const_handle item, tgui::Panel::Ptr panel);
		void addItemToEquipmentUI(const BodyPart& part, tgui::Panel::Ptr panel);

		std::string shortenPartName(const std::string& fullPartName) const;

	private:
		ContainerWrapper _container;
		entt::handle _sessionEntity;
	};
}



