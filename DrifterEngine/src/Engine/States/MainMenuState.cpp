#include "pch.h"
#include "MainMenuState.h"
#include "ProcGen/WorldGenerator.h"

drft::MainMenuState::MainMenuState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	getContext().registry.ctx().emplace<gen::WorldGenerator>().loadBiomeBlueprints("biomes.json");
	// TODO: Allow the world generator seed to be set from the main menu

	const auto& VIEW = getContext().window.getView();
	_mainMenuWindow.setSize(VIEW.getSize())
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Focused, {
			.fillColor = sf::Color::Black,
			.innerPadding = {0,0.f},
			.childPadding = {0, 64.f}
			});	
	_mainMenuWindow.setChildrenOrigin(gui::ElementPosition::TOP_CENTER, {0, 32});

	auto& cont = _mainMenuWindow.insert("Continue", gui::Button())
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
			});

	_mainMenuWindow.insert("New Game", gui::Button())
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
			});

	_mainMenuWindow.insert("Settings", gui::Button())
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

	_mainMenuWindow.insert("Exit", gui::Button())
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
			});
	
	_mainMenuWindow.setState(gui::ElementState::Focused);
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
