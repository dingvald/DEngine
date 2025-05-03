#pragma once
#include <States/State.h>

namespace drft
{
	class EntityFactory;

	class CraftingState : public State
	{
	public:
		CraftingState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;

	private:
		void setupPanelTemplate(tgui::Panel::Ptr templatePanel);
		void determineSessionEntities();
		void refreshCraftingList(tgui::PanelListBox::Ptr list);

		void refreshSessionEntityIngredients();

		void addItemToCraftingList(entt::const_handle item, tgui::Panel::Ptr panel, bool isPartial);
		void addItemIconAndNameWidgets(entt::const_handle item, tgui::Panel::Ptr panel, bool isPartial);
		void addItemRecipeWidgets(entt::const_handle item, tgui::Panel::Ptr panel, bool isPartial);
		void addIngredientWidget(entt::const_handle item, unsigned int amount, tgui::Grid::Ptr grid, int index, bool isPartial);
		void addNothingToCraftWidget(tgui::Panel::Ptr panel);

		void onCraft(entt::id_type itemEntityId);

	private:
		entt::handle _sessionEntity = {};
		std::unordered_map<std::string, int> _ingredients;
		const EntityFactory* _factory = nullptr;
	};
}



