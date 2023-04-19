#include "pch.h"
#include "InventoryState.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"

drft::InventoryState::InventoryState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	_background.setFillColor(sf::Color(0, 0, 0, 150));
	_background.setSize({ static_cast<float>(getContext().window.getView().getSize().x),
		static_cast<float>(getContext().window.getView().getSize().y)});
}

bool drft::InventoryState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackPop();
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad8 || ev.key.code == sf::Keyboard::Up)
		{
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad2 || ev.key.code == sf::Keyboard::Down)
		{
			return false;
		}
		if (ev.key.code == sf::Keyboard::Space)
		{
			return false;
		}
		break;
	}

	return false;
}

bool drft::InventoryState::update(const float dt)
{
    return false;
}

void drft::InventoryState::render(sf::RenderTarget& target)
{
	
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
