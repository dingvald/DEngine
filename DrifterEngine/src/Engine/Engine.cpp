#include "pch.h"
#include "Engine.h"
#include "States/GameState.h"
#include "States/TitleScreenState.h"
#include "States/MainMenuState.h"
#include "States/InventoryState.h"
#include "States/PauseState.h"
#include "Services/DebugInfo.h"

using namespace drft;
constexpr float TARGET_FPS = 60.0f;

drft::Engine::Engine()
	: _window(sf::VideoMode(960, 540), "Drifter Engine")
	, _showDebug(false)
{
	initialize();
}

void drft::Engine::run()
{
	sf::Clock clock;
	while (_window.isOpen())
	{
		sf::Time deltaTime = clock.restart();
		handleEvents();
		update(deltaTime.asSeconds());
		render(deltaTime.asSeconds());
		if (_stateStack.isEmpty())
		{
			this->shutDown();
		}
	}
}

void drft::Engine::initialize()
{
	loadResources();
	service::DebugInfo::instance().setFont(_fonts.get("Terminus"));
	service::DebugInfo::instance().setPosition({ 16,8 });

	registerStates();
	_stateStack.pushState(States::Title);
}

void drft::Engine::loadResources()
{
	std::string resourcePath("./resources/");
	_textures.load("Sprites", resourcePath + "Textures/simpleTileset.png");
	_fonts.load("Terminus", resourcePath + "Fonts/terminus.ttf");
}

void drft::Engine::registerStates()
{
	_stateStack.registerState<TitleScreenState>(States::Title);
	_stateStack.registerState<MainMenuState>(States::MainMenu);
	_stateStack.registerState<GameState>(States::Game);
	_stateStack.registerState<InventoryState>(States::Inventory);
	_stateStack.registerState<PauseState>(States::Pause);
}

void drft::Engine::handleEvents()
{
	sf::Event event;
	while (_window.pollEvent(event))
	{
		_stateStack.handleEvent(event);
		switch (event.type)
		{
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::F5)
				{
					_showDebug = !_showDebug;
				}
				break;
			case sf::Event::Closed:
				this->shutDown();
				break;
		}
	}
}

void drft::Engine::update(const float dt)
{
	float fps = 1.0f / dt;
	service::DebugInfo::instance().putInfo("FPS", std::to_string(fps));
	service::DebugInfo::instance().putInfo("dt", std::to_string(dt));

	_stateStack.update(dt);
}

void drft::Engine::render(const float dt)
{
	static float dtSinceRender = 0.0f;

	dtSinceRender += dt;
	if (dtSinceRender >= (1.0f / TARGET_FPS))
	{
		_window.clear();
		_stateStack.render(_window);

		if (_showDebug)
		{
			service::DebugInfo::instance().render(_window);
		}
			
		_window.display();
		dtSinceRender = 0.0f;
	}
}

void drft::Engine::shutDown()
{
	std::cout << "Closing Engine" << std::endl;
	_window.close();
}




