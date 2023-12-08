#include "pch.h"
#include "Engine.h"
#include "EngineConstants.h"
#include "States/GameState.h"
#include "States/TitleScreenState.h"
#include "States/MainMenuState.h"
#include "States/InventoryState.h"
#include "States/CraftingState.h"
#include "States/PauseState.h"
#include "States/GameOverState.h"
#include "States/WorldMapState.h"
#include "States/DisplayTextState.h"
#include "States/SelectDirectionState.h"
#include "Services/DebugInfo.h"

using namespace drft;

static const std::string RESOURCE_PATH = "./resources/";
static const std::string TEXTURE_PATH = RESOURCE_PATH + "Textures/";
static const std::string FONTS_PATH = RESOURCE_PATH + "Fonts/";

drft::Engine::Engine()
	: _window(sf::VideoMode(1280, 720), "Drifter Engine")
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
			shutDown();
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
	_textures.load("Sprites", TEXTURE_PATH + "simpleTileset.png");
	_textures.load("Icons", TEXTURE_PATH + "icons.png");
	_fonts.load("Terminus", FONTS_PATH + "terminus.ttf");
}

void drft::Engine::registerStates()
{
	_stateStack.registerState<TitleScreenState>(States::Title);
	_stateStack.registerState<MainMenuState>(States::MainMenu);
	_stateStack.registerState<GameState>(States::Game);
	_stateStack.registerState<InventoryState>(States::Inventory);
	_stateStack.registerState<CraftingState>(States::Crafting);
	_stateStack.registerState<WorldMapState>(States::Map);
	_stateStack.registerState<PauseState>(States::Pause);
	_stateStack.registerState<GameOverState>(States::GameOver);
	_stateStack.registerState<DisplayTextState>(States::DisplayText);
	_stateStack.registerState<SelectDirectionState>(States::SelectDirection);
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
	_fpsStats.sample(fps);
	_dtStats.sample(dt);
	service::DebugInfo::instance().putInfo("Avg FPS", std::to_string(_fpsStats.average()));
	service::DebugInfo::instance().putInfo("Min FPS", std::to_string(_fpsStats.min()));

	service::DebugInfo::instance().putInfo("Avg dt", std::to_string(_dtStats.average()));
	service::DebugInfo::instance().putInfo("Max dt", std::to_string(_dtStats.max()));

	_stateStack.update(dt);
	
	static float dtSinceFixedUpdate = 0.0f;
	dtSinceFixedUpdate += dt;
	if (dtSinceFixedUpdate >= (1.0f / TARGET_FPS))
	{
		_stateStack.fixedUpdate();
		dtSinceFixedUpdate = 0.0f;
	}

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




