#include "pch.h"
#include "InventoryState.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"

drft::InventoryState::InventoryState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	const auto VIEW = getContext().window.getView();

	_inventoryWindow.setSize(VIEW.getSize())
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,150),
			.outlineThickness = 0.0f,
		})
		.setChildrenAlignment(gui::ElementAlignment::CENTER)
		.insertChild("ItemList", gui::List());

	_inventoryWindow["ItemList"]
		.setSize({ VIEW.getSize().x / 3, 3*(VIEW.getSize().y / 4)})
		.setPosition({ 0,0 })
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(50,50,50,200),
			.outlineColor = sf::Color(50,50,50),
			.outlineThickness = 2.0f,
			.innerPadding = 32.0f,
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White,
			.textSize = 16
			})
		.setTextString("Inventory")
		.setTextOrigin(gui::ElementOrigin::BOTTOM_CENTER)
		.setTextPosition(gui::ElementTextPosition::TOP_CENTER)
		.setChildrenAlignment(gui::ElementAlignment::TOP_CENTER)
		.insertChild("Item1", gui::Button())
		.insertChild("Item2", gui::Button());
	
	_inventoryWindow["ItemList"]["Item1"]
		.setStyle(gui::ElementState::Idle, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White
		})
		.setStyle(gui::ElementState::Focused, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::Yellow
			})
		.setTextString("Item1");

	_inventoryWindow["ItemList"]["Item2"]
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,0),
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White
		})
		.setStyle(gui::ElementState::Focused, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::Yellow
			})
		.setTextString("Item2");

}

bool drft::InventoryState::handleEvent(const sf::Event& ev)
{
	_inventoryWindow.handleEvent(ev);

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

bool drft::InventoryState::update(const float dt)
{
	_inventoryWindow.update(dt);

    return false;
}

void drft::InventoryState::render(sf::RenderTarget& target)
{
	_inventoryWindow.render(target);
}

void drft::InventoryState::onPush()
{
	determineSessionEntities();
	setupPanels();
}

void drft::InventoryState::onPop()
{
	shutdownSessionEntities();
}

void drft::InventoryState::setupPanels()
{
	
}

void drft::InventoryState::determineSessionEntities()
{
	auto view = getContext().registry.view<component::action::OpenInventory>();
	for (auto entity : view)
	{
		_sessionEntities.push_back(entity);
	}
	assert(_sessionEntities.size() <= 2);
}

void drft::InventoryState::shutdownSessionEntities()
{
	for (auto entity : _sessionEntities)
	{
		getContext().registry.remove<component::action::OpenInventory>(entity);
	}
}
