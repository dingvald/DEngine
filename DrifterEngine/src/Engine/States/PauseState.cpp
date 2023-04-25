#include "pch.h"
#include "PauseState.h"

drft::PauseState::PauseState(StateStack& stack, StateContext& context)
    :State(stack, context)
{
	const auto& VIEW = getContext().window.getView();
	_pauseWindow.setSize(VIEW.getSize())
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,150),
			.innerPadding = 192.f,
			.childPadding = 64.f
			})
		.setChildrenOrigin(gui::ElementAlignment::TOP_CENTER)
		.insertChild("Continue", gui::Button())
		.insertChild("Settings", gui::Button())
		.insertChild("Exit", gui::Button());

	_pauseWindow["Continue"]
		.setStyle(gui::ElementState::Idle, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White,
			.textSize = 32
			})
		.setStyle(gui::ElementState::Focused, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::Yellow,
			.textSize = 32
			})
		.setTextString("Continue")
		.registerCallback(gui::ElementCallbackType::OnSelect, [this]()
			{
				this->requestStackPop();
				return true;
			});

	_pauseWindow["Settings"]
		.setStyle(gui::ElementState::Idle, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White,
			.textSize = 32
			})
		.setStyle(gui::ElementState::Focused, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::Yellow,
			.textSize = 32
			})
		.setTextString("Settings");

	_pauseWindow["Exit"]
		.setStyle(gui::ElementState::Idle, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White,
			.textSize = 32
			})
		.setStyle(gui::ElementState::Focused, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::Yellow,
			.textSize = 32
			})
		.setTextString("Exit")
		.registerCallback(gui::ElementCallbackType::OnSelect, [this]()
			{
				this->requestStackClear();
				this->requestStackPush(States::MainMenu);
				return true;
			});
}

bool drft::PauseState::handleEvent(const sf::Event& ev)
{
	_pauseWindow.handleEvent(ev);
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

bool drft::PauseState::update(const float dt)
{
	_pauseWindow.update(dt);

	return false;
}

void drft::PauseState::render(sf::RenderTarget& target)
{
	_pauseWindow.render(target);
}


