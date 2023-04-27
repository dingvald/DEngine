#include "pch.h"
#include "PauseState.h"

drft::PauseState::PauseState(StateStack& stack, StateContext& context)
    :State(stack, context)
{
	const auto& VIEW = getContext().window.getView();
	_pauseBackground.setSize(VIEW.getSize())
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,100),
			.innerPadding = {0, 192.f},
			.childPadding = {0, 64.f}
			});

	_pauseWindow.setSize(VIEW.getSize())
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Idle, {
			.innerPadding = {0, 192.f},
			.childPadding = {0, 64.f}
			});
	_pauseWindow.insert("Continue", gui::Button())
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

	_pauseWindow.insert("Settings", gui::Button())
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

	_pauseWindow.insert("Exit", gui::Button())
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

	_pauseWindow.setChildrenOrigin(gui::ElementPosition::TOP_CENTER, { 0,32 });
	_pauseWindow.layoutChildren();
}

bool drft::PauseState::handleEvent(const sf::Event& ev)
{
	_pauseBackground.handleEvent(ev);
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
	_pauseBackground.update(dt);
	_pauseWindow.update(dt);

	return false;
}

void drft::PauseState::render(sf::RenderTarget& target)
{
	_pauseBackground.render(target);
	_pauseWindow.render(target);
}


