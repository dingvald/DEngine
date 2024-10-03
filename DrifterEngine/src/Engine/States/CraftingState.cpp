#pragma optimize("", off)

#include "pch.h"
#include "CraftingState.h"

#include "Components/Components.h"
#include "Components/MyCraftableItemsComponent.h"
#include "Components/CraftableComponent.h"
#include "Components/RenderComponent.h"
#include "Components/ContainerComponent.h"

#include <Systems/Gameplay/Actions/CraftItemSystem.h>

#include "Factory/EntityFactory.h"
#include "Utility/EntityHelpers.h"
#include "Utility/TextureAtlas.h"
#include <Utility/TGUIHelpers.h>
#include "Systems/Helpers/ItemDatabase.h"

using namespace entt::literals;

static constexpr float CRAFTING_WINDOW_WIDTH = 512.f;
static constexpr float CRAFTING_WINDOW_HEIGHT = 480.f;
static constexpr float DISTANCE_BETWEEN_ITEMS_AND_REQUIREMENTS = 180.f;

static const char* CraftablesListWidget = "list_box";
static const char* ListEntryWidget = "list_entry";
static const char* ListEntryButtonWidget = "list_entry_button";
static const char* ItemNameWidget = "item_name";
static const char* ItemIconWidget = "icon";
static const char* NameGridWidget = "name_grid";
static const char* RecipeGridWidget = "recipe_grid";

drft::CraftingState::CraftingState(StateStack& stack, StateContext& context, tgui::Group::Ptr gui)
    : State(stack, context, gui)
{
	_factory = &getContext().registry.ctx().get<const EntityFactory&>();
	determineSessionEntities();

	auto list = tgui::PanelListBox::create();
	_gui->add(list, CraftablesListWidget);
	list->setOrigin(0.5f, 0.5f);
	list->setPosition("50%, 50%");
	list->setSize(tgui::bindWidth(_gui) * 0.5f, tgui::bindHeight(_gui) * 0.5f);
	list->setItemsHeight(48.f);

	auto templatePanel = list->getPanelTemplate();
	setupCraftableEntryTemplate(templatePanel);
	
	_gui->setNavigationDown(list);
	_gui->setNavigationUp(list);
	list->onFocus([this, list]() { tryFocusFirstItem(list); });

	refreshCraftingList(list);
}
88
bool drft::CraftingState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackPop();
			return false;
		}
		break;
	}

	return false;
}

void drft::CraftingState::setupCraftableEntryTemplate(tgui::Panel::Ptr templatePanel)
{
	auto button = tgui::Button::create();
	templatePanel->add(button, ListEntryButtonWidget);
	button->setSize("100%, 100%");

	auto icon = tgui::Picture::create();
	templatePanel->add(icon, ItemIconWidget);
	icon->setSize(32, 48);
	icon->setOrigin(0.f, 0.5f);
	icon->setPosition(4, "50%");

	auto text = tgui::Label::create();
	templatePanel->add(text, ItemNameWidget);
	text->setVerticalAlignment(tgui::VerticalAlignment::Center);
	text->setTextSize(16);
	text->setOrigin(0.f, 0.5f);
	text->setPosition(icon->getSize().x + 4, "50%");

	auto recipe_grid = tgui::Grid::create();
	templatePanel->add(recipe_grid, RecipeGridWidget);
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
	list->removeAllItems();
	list->setFocused(false);

	if (auto craftables = _sessionEntity.try_get<MyCraftableItemsComponent>())
	{
		const auto& prototypeReg = _factory->prototypes();
		for (auto&& craftable : craftables->craftables)
		{
			auto panel = list->addItem();
			addItemToCraftingList({ prototypeReg, craftable }, panel);
		}
		for (auto&& partial : craftables->partialCraftables)
		{
			auto panel = list->addItem();
			addItemToCraftingList({ prototypeReg, partial }, panel);
		}
	}
	setupNavigationGraph(list);
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

void drft::CraftingState::addItemToCraftingList(entt::const_handle item, tgui::Panel::Ptr panel)
{
	auto name = util::getEntityName(item);
	addItemIconAndNameWidgets(name, item, panel);
	addItemRecipeWidgets(item, panel);

	auto button = panel->get<tgui::Button>(ListEntryButtonWidget);
	panel->onFocus([button]() { button->setFocused(true); });
	panel->onUnfocus([button]() {button->setFocused(false); });

	button->onPress([this, name] { onCraft(name); });
}

void drft::CraftingState::addItemIconAndNameWidgets(const std::string& name, entt::const_handle item, tgui::Panel::Ptr panel)
{
	auto render = util::getRenderData(item);
	auto rect = getContext().textures.getUV(render.texture, render.uvSize, render.uvCoords);
	tgui::Texture texture{ name, toUIntRect(rect) };
	texture.setColor(render.color);

	auto icon = panel->get<tgui::Picture>(ItemIconWidget);
	icon->getRenderer()->setTexture(texture);
	icon->setIgnoreMouseEvents(true);

	auto text = panel->get<tgui::Label>(ItemNameWidget);
	text->setText(name);
	text->setIgnoreMouseEvents(true);
}

void drft::CraftingState::addItemRecipeWidgets(entt::const_handle item, tgui::Panel::Ptr panel)
{
	auto& craftable = item.get<CraftableComponent>();
	auto grid = panel->get<tgui::Grid>(RecipeGridWidget);
	grid->setIgnoreMouseEvents(true);

	int index = 0;
	for (auto&& [ingredient, amount] : craftable.recipe)
	{
		auto prototype = _factory->get(ingredient);
		addIngredientWidget(prototype, amount, grid, index);
		index++;
	}
}

void drft::CraftingState::addIngredientWidget(entt::const_handle item, unsigned int amount, tgui::Grid::Ptr grid, int index)
{
	auto sub_grid = tgui::Grid::create();
	auto ingredientName = util::getEntityName(item);

	auto render = util::getRenderData(item);
	auto rect = getContext().textures.getUV(render.texture, render.uvSize, render.uvCoords);
	tgui::Texture texture{ ingredientName, toUIntRect(rect)};
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

	grid->add(sub_grid, ingredientName);
	grid->setWidgetCell(sub_grid, 0, index);
}

void drft::CraftingState::onCraft(const std::string& name)
{
	if (system::CraftItemSystem::craftItem(_sessionEntity, name))
	{
		refreshCraftingList(_gui->get<tgui::PanelListBox>(CraftablesListWidget));
	}
}

void drft::CraftingState::tryFocusFirstItem(tgui::PanelListBox::Ptr list)
{
	if (auto first = list->getItemByIndex(0))
	{
		first->setFocused(true);
	}
}

void drft::CraftingState::setupNavigationGraph(tgui::PanelListBox::Ptr list)
{
	if (list->getItemCount() == 0) return;

	tgui::Panel::Ptr previous = list->getItemByIndex(list->getItemCount() - 1);
	for (auto&& panel : list->getItems())
	{
		panel->setNavigationUp(previous);
		previous->setNavigationDown(panel);
		previous = panel;
	}
}


