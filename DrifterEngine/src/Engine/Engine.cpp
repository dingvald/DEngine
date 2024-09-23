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
#include <Utility/StandardLogger.h>

using namespace drft;

static const std::filesystem::path RESOURCE_PATH = std::filesystem::current_path() / "resources";
static const std::filesystem::path TEXTURE_PATH = RESOURCE_PATH / "Textures";
static const std::filesystem::path FONTS_PATH = RESOURCE_PATH / "Fonts";
static const std::filesystem::path ICONS_PATH = RESOURCE_PATH / "Icon";
static const std::filesystem::path THEMES_PATH = RESOURCE_PATH / "GUI" / "Themes";

static const std::filesystem::path DEFAULT_THEME = "debug_theme.txt";

static const float TARGET_DT = (1.0f / TARGET_FPS);

static const unsigned int WINDOW_WIDTH = 1280;
static const unsigned int WINDOW_HEIGHT = 720;

static const float DEBUG_X_POSITION = WINDOW_WIDTH - 256;
static const float DEBUG_Y_POSITION = 16;

static const float MOUSE_TIMEOUT_TIME = 1.5f; // in seconds


drft::Engine::Engine()
	: _window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Drifter Engine")
	, _gui(_window)
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
	setWindowIcon();
	loadResources();
	service::DebugInfo::instance().setFont(_fonts.get("Terminus"));
	service::DebugInfo::instance().setPosition({ DEBUG_X_POSITION, DEBUG_Y_POSITION });
	registerStates();

	_gui.setKeyboardNavigationEnabled(true);

	_stateStack.pushState(States::Title);
}

void drft::Engine::setWindowIcon()
{
	sf::Image icon;
	const std::filesystem::path iconPath = ICONS_PATH / "drifter-project-icon.png";
	if (icon.loadFromFile(iconPath.string()))
	{
		_window.setIcon(32, 32, icon.getPixelsPtr());
	}
	else
	{
		warning_logger << "Warning: Could not set window icon:" << std::endl;
		warning_logger << "File " << iconPath << " not found." << std::endl;
	}
}

void drft::Engine::loadResources()
{
	_textures.load(TEXTURE_PATH);

	std::string terminus_font_path = (FONTS_PATH / "terminus.ttf").string();
	_fonts.load("Terminus", terminus_font_path);
	tgui::Font globalFont = { terminus_font_path };
	tgui::Font::setGlobalFont(globalFont);

	std::string default_theme_path = (THEMES_PATH / DEFAULT_THEME).string();
	tgui::Theme::setDefault(default_theme_path);

	tgui::Texture::setBackendTextureLoader(
		[this](tgui::BackendTexture& backendTexture, const tgui::String&, bool) -> bool
		{
			tgui::BackendTextureSFML& sfmlTexture = static_cast<tgui::BackendTextureSFML&>(backendTexture);
			sfmlTexture.replaceInternalTexture(_textures.getTexture());
			return true;
		});
}

void drft::Engine::registerStates()
{
	_stateStack.registerState<TitleScreenState>(States::Title);
	_stateStack.registerState<MainMenuState>(States::MainMenu);
	_stateStack.registerState<GameState>(States::Game);
	_stateStack.registerState<InventoryState>(States::Inventory); // TODO: Move to substate of game
	_stateStack.registerState<CraftingState>(States::Crafting); // TODO: Move to substate of game
	_stateStack.registerState<WorldMapState>(States::Map); // TODO: Move to substate of game
	_stateStack.registerState<PauseState>(States::Pause);
	_stateStack.registerState<GameOverState>(States::GameOver);
	_stateStack.registerState<SelectDirectionState>(States::SelectDirection); // TODO: Move to substate of game
	_stateStack.registerState<SelectTargetState>(States::SelectTarget); // TODO: Move to substate of game
}

void drft::Engine::handleEvents()
{
	sf::Event event;
	while (_window.pollEvent(event))
	{
		switch (event.type)
		{
			case sf::Event::KeyPressed:
				handleKeyboardEvents(event);
				break;
			case sf::Event::Closed:
				shutDown();
				break;
			default:
				handleMouseEvents(event);
				break;
		}

		passEventToGui(event);
		passEventToState(event);
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
		_gui.draw();

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

void drft::Engine::handleMouseEvents(sf::Event event)
{
	switch (event.type)
	{
	// If you so much as touch the mouse the engine swaps to mouse input for the game
	case sf::Event::MouseButtonPressed:
	case sf::Event::MouseButtonReleased:
	case sf::Event::MouseMoved:
	case sf::Event::MouseWheelMoved:
	case sf::Event::MouseWheelScrolled:
		if (_navigationType == NavigationType::Keyboard)
			swapToMouse();
		break;
	default:
		break;
	}
}

void drft::Engine::handleKeyboardEvents(sf::Event event)
{
	using Key = sf::Keyboard;
	switch (event.key.code)
	{
	case Key::F5:
		toggleDebug();
		break;
	case Key::F11:
		toggleFullscreen();
		break;
	case Key::Up:
	case Key::Down:
	case Key::Left:
	case Key::Right:
	case Key::Numpad8:
	case Key::Numpad6:
	case Key::Numpad4:
	case Key::Numpad2:
		if (_navigationType == NavigationType::Mouse) 
			swapToKeyboard();
		break;
	default:
		break;
	}
}

void drft::Engine::passEventToGui(sf::Event event)
{
	// Convert numpad directions to keyboard arrow directions
	if (event.type == sf::Event::KeyPressed)
	{
		using Key = sf::Keyboard;
		switch (event.key.code)
		{
		case Key::Numpad8:
			event.key.code = Key::Up;
			break;
		case Key::Numpad6:
			event.key.code = Key::Right;
			break;
		case Key::Numpad4:
			event.key.code = Key::Left;
			break;
		case Key::Numpad2:
			event.key.code = Key::Down;
			break;
		default:
			break;
		}
	}
	_gui.handleEvent(event);
}

void drft::Engine::passEventToState(sf::Event event)
{
	_stateStack.handleEvent(event);
}

void drft::Engine::swapToMouse()
{
	_window.setMouseCursorVisible(true);
	sf::Event ev{};
	ev.type = sf::Event::MouseEntered;
	_gui.handleEvent(ev);
	_gui.unfocusAllWidgets();
	_navigationType = NavigationType::Mouse;
}

void drft::Engine::swapToKeyboard()
{
	_window.setMouseCursorVisible(false);

	sf::Event ev{};
	ev.type = sf::Event::MouseLeft;
	_gui.handleEvent(ev);
	auto container = _gui.getContainer();
	for (auto&& child : container->getWidgets())
	{
		if (!child->isVisible()) continue;
		child->setFocused(true);
		break;
	}
	_navigationType = NavigationType::Keyboard;
}

void drft::Engine::toggleFullscreen()
{
	if (_isFullScreen)
	{
		_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Drifter Engine");
		_isFullScreen = false;
		std::cout << "Window set to windowed mode" << std::endl;
	}
	else
	{
		_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Drifter Engine", sf::Style::Fullscreen);
		_isFullScreen = true;
		std::cout << "Window set to fullscreen mode" << std::endl;
	}
}

void drft::Engine::toggleDebug()
{
	_showDebug = !_showDebug;
}





