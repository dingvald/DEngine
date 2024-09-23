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

static constexpr float CRAFTING_WINDOW_WIDTH = 352.f;
static constexpr float CRAFTING_WINDOW_HEIGHT = 256.f;
static constexpr float DISTANCE_BETWEEN_ITEMS_AND_REQUIREMENTS = 180.f;

static const char* CraftablesListWidget = "list_box";
static const char* ListEntryWidget = "list_entry";
static const char* ItemNameWidget = "item_name";
static const char* ItemIconWidget = "icon";
static const char* RecipeGridWidget = "recipe";

drft::CraftingState::CraftingState(StateStack& stack, StateContext& context, tgui::Group::Ptr gui)
    : State(stack, context, gui)
{
	_factory = &getContext().registry.ctx().get<const EntityFactory&>();
	determineSessionEntities();

	auto list = tgui::PanelListBox::create();
	list->setOrigin(0.5f, 0.5f);
	list->setPosition("50%, 50%");
	list->setSize("50%, 50%");

	// create template ////////////////////////////////////
	auto templatePanel = list->getPanelTemplate();
	setupCraftableEntryTemplate(templatePanel);
	////////////////////////////////////////////////////////
	
	list->setItemsHeight(48.f);

	_gui->add(list, CraftablesListWidget);

	_gui->setNavigationDown(list);
	_gui->setNavigationUp(list);

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
			return false;
		}
		break;
	}

	return false;
}

void drft::CraftingState::setupCraftableEntryTemplate(tgui::Panel::Ptr templatePanel)
{
	auto layout = tgui::HorizontalLayout::create();
	layout->setSize("100%, 100%");

	auto name_grid = tgui::Grid::create();

	auto text = tgui::Label::create();
	text->setVerticalAlignment(tgui::VerticalAlignment::Center);
	text->setHorizontalAlignment(tgui::HorizontalAlignment::Left);
	text->setTextSize(16);

	auto icon = tgui::Picture::create();
	icon->setSize(32, 48);

	name_grid->add(icon, ItemIconWidget);
	name_grid->setWidgetCell(icon, 0, 0);
	name_grid->add(text, ItemNameWidget);
	name_grid->setWidgetCell(text, 0, 1);

	auto recipe_grid = tgui::Grid::create();

	layout->add(name_grid, 1);
	layout->addSpace(2);
	layout->add(recipe_grid, 2, RecipeGridWidget);

	templatePanel->add(layout);
}

void drft::CraftingState::determineSessionEntities()
{
	auto craftingView = getContext().registry.view<component::action::OpenCrafting>();
	_sessionEntity = { getContext().registry, craftingView.front() };
}

void drft::CraftingState::shutdownSessionEntities()
{
	_sessionEntity.remove<component::action::OpenCrafting>();
}

void drft::CraftingState::refreshCraftingList(tgui::PanelListBox::Ptr list)
{
	refreshSessionEntityIngredients();
	list->removeAllItems();
	const auto& prototypeReg = _factory->prototypes();

	if (auto craftables = _sessionEntity.try_get<MyCraftableItemsComponent>())
	{
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

	panel->onClick([this, name] { onCraft(name); });
}

void drft::CraftingState::addItemIconAndNameWidgets(const std::string& name, entt::const_handle item, tgui::Panel::Ptr panel)
{
	auto render = util::getRenderData(item);
	auto rect = getContext().textures.getUV(render.texture, render.uvSize, render.uvCoords);
	tgui::Texture texture{ name, toUIntRect(rect) };
	texture.setColor(render.color);

	auto icon = panel->get<tgui::Picture>(ItemIconWidget);
	icon->getRenderer()->setTexture(texture);

	auto text = panel->get<tgui::Label>(ItemNameWidget);

	text->setText(name);
}

void drft::CraftingState::addItemRecipeWidgets(entt::const_handle item, tgui::Panel::Ptr panel)
{
	auto grid = panel->get<tgui::Grid>(RecipeGridWidget);
	auto& craftable = item.get<CraftableComponent>();
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
	sub_grid->addWidget(label, 0, 1);

	grid->addWidget(sub_grid, 0, index);
}

void drft::CraftingState::onCraft(const std::string& name)
{
	if (system::CraftItemSystem::craftItem(_sessionEntity, name))
	{
		refreshCraftingList(_gui->get<tgui::PanelListBox>(CraftablesListWidget));
	}
}

