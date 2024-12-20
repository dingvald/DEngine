#pragma optimize("", off)

#include "pch.h"
#include "CraftingState.h"

#include "Components/Components.h"
#include "Components/MyCraftableItemsComponent.h"
#include "Components/CraftableComponent.h"
#include "Components/RenderComponent.h"
#include "Components/ContainerComponent.h"

#include <Systems/Actions/CraftItemSystem.h>

#include "Factory/EntityFactory.h"
#include "Utility/EntityHelpers.h"
#include "Utility/TextureAtlas.h"
#include <Utility/TGUIHelpers.h>
#include "Systems/Helpers/ItemDatabase.h"

using namespace entt::literals;

static constexpr float CRAFTING_WINDOW_WIDTH = 512.f;
static constexpr float CRAFTING_WINDOW_HEIGHT = 480.f;
static constexpr float DISTANCE_BETWEEN_ITEMS_AND_REQUIREMENTS = 180.f;

static const char* w_CraftablesList		= "list_box";
static const char* w_EntryButton		= "list_entry_button";
static const char* w_EntryName			= "item_name";
static const char* w_EntryIcon			= "icon";
static const char* w_RecipeGrid			= "recipe_grid";

drft::CraftingState::CraftingState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	_factory = &getContext().registry.ctx().get<const EntityFactory&>();
	determineSessionEntities();

	auto list = tgui::PanelListBox::create();
	_guiGroup->add(list, w_CraftablesList);
	list->setOrigin(0.5f, 0.5f);
	list->setPosition("50%, 50%");
	list->setSize(tgui::bindWidth(_guiGroup) * 0.5f, tgui::bindHeight(_guiGroup) * 0.5f);
	list->setItemsHeight(48.f);

	auto templatePanel = list->getPanelTemplate();
	setupPanelTemplate(templatePanel);

	refreshCraftingList(list);
}

bool drft::CraftingState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackPop();
			return true;
		}
		break;
	}

	return false;
}

void drft::CraftingState::setupPanelTemplate(tgui::Panel::Ptr templatePanel)
{
	auto icon = tgui::Picture::create();
	templatePanel->add(icon, w_EntryIcon);
	icon->setSize(32, 48);
	icon->setOrigin(0.f, 0.5f);
	icon->setPosition(4, "50%");

	auto text = tgui::Label::create();
	templatePanel->add(text, w_EntryName);
	text->setVerticalAlignment(tgui::VerticalAlignment::Center);
	text->setTextSize(16);
	text->setOrigin(0.f, 0.5f);
	text->setPosition(icon->getSize().x + 4, "50%");

	auto recipe_grid = tgui::Grid::create();
	templatePanel->add(recipe_grid, w_RecipeGrid);
	recipe_grid->setOrigin(0.f, 0.5f);
	recipe_grid->setPosition("50%, 50%");
}

void drft::CraftingState::determineSessionEntities()
{
	auto craftingView = getContext().registry.view<component::action::OpenCrafting>();
	_sessionEntity = { getContext().registry, craftingView.front() };
	_sessionEntity.remove<component::action::OpenCrafting>();
}

void drft::CraftingState::refreshCraftingList(tgui::PanelListBox::Ptr list)
{
	refreshSessionEntityIngredients();
	auto index = GuiHelpers::getFocusedIndex(list);
	list->removeAllItems();
	list->setFocused(false);

	if (auto craftables = _sessionEntity.try_get<MyCraftableItemsComponent>())
	{
		const auto& prototypeReg = _factory->prototypes();
		for (auto&& craftable : craftables->craftables)
		{
			auto panel = list->addItem();
			addItemToCraftingList({ prototypeReg, craftable }, panel, false);
		}
		for (auto&& partial : craftables->partialCraftables)
		{
			auto panel = list->addItem();
			addItemToCraftingList({ prototypeReg, partial }, panel, true);
		}
	}

	GuiHelpers::setupNavigationGraph(list);
	list->onFocus([this, index, list]() {
		if (getContext().controls.navigation == NavigationType::Keyboard)
		{
			GuiHelpers::tryFocusItem(list, std::max(0, index));
		}
		});

	if (getContext().controls.navigation == NavigationType::Keyboard)
	{
		GuiHelpers::tryFocusItem(list, std::max(0, index));
	}

	if (list->getItemCount() == 0)
	{
		auto panel = list->addItem();
		addNothingToCraftWidget(panel);
	}
}

void drft::CraftingState::refreshSessionEntityIngredients()
{
	_ingredients.clear();
	if (auto container = _sessionEntity.try_get<ContainerComponent>())
	{
		for (auto&& itemID : container->contents)
		{
			auto itemEntity = ItemDatabase::getEntityFromItemID(itemID);
			entt::const_handle handle = { getContext().registry, itemEntity };
			auto itemName = util::getEntityName(handle);

			_ingredients[itemName]++;
		}
	}
}

void drft::CraftingState::addItemToCraftingList(entt::const_handle item, tgui::Panel::Ptr panel, bool isPartial)
{
	auto name = util::getEntityName(item);
	addItemIconAndNameWidgets(name, item, panel, isPartial);
	addItemRecipeWidgets(item, panel, isPartial);
}

void drft::CraftingState::addItemIconAndNameWidgets(const std::string& name, entt::const_handle item, tgui::Panel::Ptr panel, bool isPartial)
{
	auto render = util::getRenderData(item);
	auto rect = getContext().textures.getUV(render.texture, render.uvSize, render.uvCoords);
	tgui::Texture texture{ name, GuiHelpers::toUIntRect(rect) };
	texture.setColor(render.color);

	auto icon = panel->get<tgui::Picture>(w_EntryIcon);
	icon->getRenderer()->setTexture(texture);
	icon->setIgnoreMouseEvents(true);

	auto text = panel->get<tgui::Label>(w_EntryName);
	text->setText(name);
	text->setIgnoreMouseEvents(true);

	if (isPartial)
	{
		text->getRenderer()->setTextColor({ 100, 100, 100 });
	}

	auto button = GuiHelpers::buttonizePanel(panel, text);

	button->onPress([this, name](){ onCraft(name); });
}

void drft::CraftingState::addItemRecipeWidgets(entt::const_handle item, tgui::Panel::Ptr panel, bool isPartial)
{
	auto& craftable = item.get<CraftableComponent>();
	auto grid = panel->get<tgui::Grid>(w_RecipeGrid);
	grid->setIgnoreMouseEvents(true);

	int index = 0;
	for (auto&& [ingredient, amount] : craftable.recipe)
	{
		auto prototype = _factory->get(ingredient);
		addIngredientWidget(prototype, amount, grid, index, isPartial);
		index++;
	}
}

void drft::CraftingState::addIngredientWidget(entt::const_handle item, unsigned int amount, tgui::Grid::Ptr grid, int index, bool isPartial)
{
	auto sub_grid = tgui::Grid::create();
	auto ingredientName = util::getEntityName(item);

	auto render = util::getRenderData(item);
	auto rect = getContext().textures.getUV(render.texture, render.uvSize, render.uvCoords);
	tgui::Texture texture{ ingredientName, GuiHelpers::toUIntRect(rect)};
	texture.setColor(render.color);

	auto icon = tgui::Picture::create(texture);
	icon->setSize(32, 48);
	sub_grid->addWidget(icon, 0, 0);

	int sessionEntityAmount = _ingredients.contains(ingredientName) ? _ingredients.at(ingredientName) : 0;

	auto label = tgui::Label::create();
	label->setText(std::format("{}/{}", sessionEntityAmount, amount));
	label->setTextSize(16);
	label->setIgnoreMouseEvents(true);
	sub_grid->addWidget(label, 0, 1);
	sub_grid->setIgnoreMouseEvents(true);

	if (isPartial)
	{
		label->getRenderer()->setTextColor({ 100, 100, 100 });
	}

	grid->add(sub_grid, ingredientName);
	grid->setWidgetCell(sub_grid, 0, index);
}

void drft::CraftingState::addNothingToCraftWidget(tgui::Panel::Ptr panel)
{
	auto text = panel->get<tgui::Label>(w_EntryName);
	text->setText("Nothing to craft.");
	text->getRenderer()->setTextColor(tgui::Color{ 100, 100, 100 });
	text->setIgnoreMouseEvents(true);
}

void drft::CraftingState::onCraft(const std::string& name)
{
	if (system::CraftItemSystem::craftItem(_sessionEntity, name))
	{
		refreshCraftingList(getContext().gui.get<tgui::PanelListBox>(w_CraftablesList));
	}
}


