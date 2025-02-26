#include "pch.h"
#include "SelectDirectionState.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Components/Components.h"
#include "Utility/GetTextCenter.h"

drft::SelectDirectionState::SelectDirectionState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	
}

bool drft::SelectDirectionState::handleEvent(const sf::Event& ev)
{
	auto selectDirectionView = getContext().registry.view<component::action::SelectDirection>();
	auto& selectDirection = getContext().registry.get<component::action::SelectDirection>(selectDirectionView.front());

	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Numpad8)
		{
			if (selectDirection.onDirectionSelect(sf::Vector2i(0, -1)))
			{
				requestStackPop();
			}
			return true;
		}
		if (ev.key.code == sf::Keyboard::Numpad2)
		{
			if (selectDirection.onDirectionSelect(sf::Vector2i(0, 1)))
			{
				requestStackPop();
			}
			return true;
		}
		if (ev.key.code == sf::Keyboard::Numpad6)
		{
			if (selectDirection.onDirectionSelect(sf::Vector2i(1, 0)))
			{
				requestStackPop();
			}
			return true;
		}
		if (ev.key.code == sf::Keyboard::Numpad4)
		{
			if (selectDirection.onDirectionSelect(sf::Vector2i(-1, 0)))
			{
				requestStackPop();
			}
			return true;
		}
		if (ev.key.code == sf::Keyboard::Numpad7)
		{
			if (selectDirection.onDirectionSelect(sf::Vector2i(-1, -1)))
			{
				requestStackPop();
			}
			return true;
		}
		if (ev.key.code == sf::Keyboard::Numpad9)
		{
			if (selectDirection.onDirectionSelect(sf::Vector2i(1, -1)))
			{
				requestStackPop();
			}
			return true;
		}
		if (ev.key.code == sf::Keyboard::Numpad3)
		{
			if (selectDirection.onDirectionSelect(sf::Vector2i(1, 1)))
			{
				requestStackPop();
			}
			return true;
		}
		if (ev.key.code == sf::Keyboard::Numpad1)
		{
			if (selectDirection.onDirectionSelect(sf::Vector2i(-1, 1)))
			{
				requestStackPop();
			}
			return true;
		}
		if (ev.key.code == sf::Keyboard::Numpad5)
		{
			if (selectDirection.onDirectionSelect(sf::Vector2i(0, 0)))
			{
				requestStackPop();
			}
			return true;
		}
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackPop();
			return true;
		}
		break;
	}
    return false;
}

void drft::SelectDirectionState::onPush()
{
	
}

void drft::SelectDirectionState::onPop()
{
	getContext().registry.clear<component::action::SelectDirection>();
}
