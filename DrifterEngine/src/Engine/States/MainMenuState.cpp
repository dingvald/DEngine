#include "pch.h"
#include "MainMenuState.h"

drft::MainMenuState::MainMenuState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	const auto& VIEW = getContext().window.getView();
	_mainMenuWindow.setSize(VIEW.getSize())
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color::Black,
			.innerPadding = 128.f,
			.childPadding = 64.f
			})
		.setChildrenAlignment(gui::ElementAlignment::TOP_CENTER)
		.insertChild("Continue", gui::Button())
		.insertChild("New Game", gui::Button())
		.insertChild("Settings", gui::Button())
		.insertChild("Exit", gui::Button());

	_mainMenuWindow["Continue"]
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
		.setStyle(gui::ElementState::Unselectable, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color(50,50,50),
			.textSize = 32
			})
		.setTextString("Continue")
		.registerCallback(gui::ElementCallbackType::OnIsSelectable, [this]() 
			{
				return this->hasSaveFile();
			})
		.registerCallback(gui::ElementCallbackType::OnSelect, [this]() 
			{
				this->requestStackClear();
				this->requestStackPush(States::Game);
				return true;
			}
		);

	_mainMenuWindow["New Game"]
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
		.setTextString("New Game")
		.registerCallback(gui::ElementCallbackType::OnSelect, [this]()
			{
				std::filesystem::remove_all(".\\data\\savegame\\");
				this->requestStackClear();
				this->requestStackPush(States::Game);
				return true;
			}
		);

	_mainMenuWindow["Settings"]
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

	_mainMenuWindow["Exit"]
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
				return true;
			}
		);
	
}

bool drft::MainMenuState::handleEvent(const sf::Event& ev)
{
	_mainMenuWindow.handleEvent(ev);
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackClear();
			return false;
		}
		break;
	}

	return false;
}

bool drft::MainMenuState::update(const float dt)
{
	_mainMenuWindow.update(dt);

	return false;
}

void drft::MainMenuState::render(sf::RenderTarget& target)
{
	_mainMenuWindow.render(target);
}

bool drft::MainMenuState::hasSaveFile() const
{
	return std::filesystem::exists(".\\data\\savegame\\");
}
