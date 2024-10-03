#pragma once
#include "State.h"
#include "GUI/GUIElement.h"

namespace drft
{
	class EntityFactory;

	class CraftingState : public State
	{
	public:
		CraftingState(StateStack& stack, StateContext& context, tgui::Group::Ptr gui);

		bool handleEvent(const sf::Event& ev) override;

	private:
		void setupCraftableEntryTemplate(tgui::Panel::Ptr templatePanel);
		void determineSessionEntities();
		void refreshCraftingList(tgui::PanelListBox::Ptr list);

		void refreshSessionEntityIngredients();

		void addItemToCraftingList(entt::const_handle item, tgui::Panel::Ptr panel);
		void addItemIconAndNameWidgets(const std::string& name, entt::const_handle item, tgui::Panel::Ptr panel);
		void addItemRecipeWidgets(entt::const_handle item, tgui::Panel::Ptr panel);
		void addIngredientWidget(entt::const_handle item, unsigned int amount, tgui::Grid::Ptr grid, int index);

		void onCraft(const std::string& name);
		void tryFocusFirstItem(tgui::PanelListBox::Ptr list);
		void setupNavigationGraph(tgui::PanelListBox::Ptr list);

	private:
		entt::handle _sessionEntity = {};
		std::unordered_map<std::string, int> _ingredients;
		const EntityFactory* _factory = nullptr;
	};
}



