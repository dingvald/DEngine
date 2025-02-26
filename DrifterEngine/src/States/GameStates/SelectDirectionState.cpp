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

	if (const auto keypressed = ev.getIf<sf::Event::KeyPressed>())
	{
		if (keypressed->code == sf::Keyboard::Key::Escape)
		{
			requestStackPop();
			return true;
		}
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
