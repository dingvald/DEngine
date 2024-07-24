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
#include "States/SelectDirectionState.h"
#include "States/SelectTargetState.h"
#include "Services/DebugInfo.h"
#include "Utility/TextureAtlas.h"

using namespace drft;

static const std::string RESOURCE_PATH = "./resources/";
static const std::string TEXTURE_PATH = RESOURCE_PATH + "Textures/";
static const std::string FONTS_PATH = RESOURCE_PATH + "Fonts/";

static const float TARGET_DT = (1.0f / TARGET_FPS);

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
	_textures.load(TEXTURE_PATH);
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
	_stateStack.registerState<SelectDirectionState>(States::SelectDirection);
	_stateStack.registerState<SelectTargetState>(States::SelectTarget);
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
	_stateStack.update(dt);
	
	_dtSinceLastFixedUpdate += dt;
	if (_dtSinceLastFixedUpdate > TARGET_DT)
	{
		_stateStack.fixedUpdate();
		_dtSinceLastFixedUpdate = 0.f;
	}

}

void drft::Engine::render(const float dt)
{
	_dtSinceLastRender += dt;
	if (_dtSinceLastRender > TARGET_DT)
	{
		_window.clear();
		_stateStack.render(_window);

		if (_showDebug)
		{
			service::DebugInfo::instance().render(_window);
		}
			
		_window.display();
		_dtSinceLastRender = 0.f;
	}
}

void drft::Engine::shutDown()
{
	std::cout << "Closing Engine" << std::endl;
	_window.close();
}




