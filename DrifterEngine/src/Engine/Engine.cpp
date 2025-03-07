#include "pch.h"
#include "Engine.h"
#include "EngineConstants.h"
#include <Engine/CommonEngineDirectories.h>

#include <States/GameState.h>
#include <States/TitleScreenState.h>
#include <States/MainMenuState.h>
#include <States/PauseState.h>
#include <States/SettingsState.h>
#include <States/Settings/KeybindingState.h>

#include "Utility/TextureAtlas.h"
#include <Utility/StandardLogger.h>
#include <JSON/JSONHelpers.h>

using namespace drft;

static const std::filesystem::path DEFAULT_THEME = "drifter_theme.txt";

static const unsigned int WINDOW_WIDTH = 2560;
static const unsigned int WINDOW_HEIGHT = 1440;

static const float DEBUG_X_POSITION = WINDOW_WIDTH - 256;
static const float DEBUG_Y_POSITION = 16;

static const float MOUSE_TIMEOUT_TIME = 1.5f; // in seconds

static constexpr int UPDATES_PER_FRAME_LIMIT = 10;


drft::Engine::Engine()
	: _window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Drifter Engine")
	, _gui(_window)
	, _showDebug(false)
{
	initialize();
}

void drft::Engine::run()
{
	sf::Clock clock;
	float lag = 0.0f;

	while (_window.isOpen())
	{
		const float secondsPassed = clock.restart().asSeconds();
		lag += std::min(secondsPassed, SECONDS_PER_FRAME * UPDATES_PER_FRAME_LIMIT);

		handleEvents();

		while (lag >= SECONDS_PER_FRAME)
		{
			update();
			lag -= SECONDS_PER_FRAME;
		}
		
		render(secondsPassed);

		if (_stateStack.isEmpty())
		{
			shutDown();
		}
	}
}

void drft::Engine::initialize()
{
	std::cout << "Initializing Engine..." << std::endl;
	std::cout << "Working Directory: " << WORKING_DIRECTORY << std::endl;

	setWindowIcon();
	loadResources();
	loadDefaultKeybindings();
	loadSavedKeybindings();
	setupActionMap();
	registerStates();

	_gui.setKeyboardNavigationEnabled(true);

	_stateStack.pushState(States::Title);
}

void drft::Engine::setWindowIcon()
{
	sf::Image icon;
	const std::filesystem::path iconPath = ICONS_DIRECTORY / "drifter-project-icon.png";
	if (icon.loadFromFile(iconPath.string()))
	{
		_window.setIcon({ 32, 32 }, icon.getPixelsPtr());
	}
	else
	{
		warning_logger << "Warning: Could not set window icon:" << std::endl;
		warning_logger << "File " << iconPath << " not found." << std::endl;
	}
}

void drft::Engine::loadResources()
{
	_textures.load(TEXTURES_DIRECTORY);

	std::string terminus_font_path = (FONTS_DIRECTORY / "terminus.ttf").string();
	_fonts.load("Terminus", terminus_font_path);
	tgui::Font globalFont = { terminus_font_path };
	tgui::Font::setGlobalFont(globalFont);

	std::string default_theme_path = (THEMES_DIRECTORY / DEFAULT_THEME).string();
	tgui::Theme::setDefault(default_theme_path);

	// Needed so TGUI can use my texture atlas for sprites
	tgui::Texture::setBackendTextureLoader(
		[this](tgui::BackendTexture& backendTexture, const tgui::String&, bool) -> bool
		{
			tgui::BackendTextureSFML& sfmlTexture = static_cast<tgui::BackendTextureSFML&>(backendTexture);
			sfmlTexture.replaceInternalTexture(_textures.getTexture());
			return true;
		});
}

void drft::Engine::loadSavedKeybindings()
{
	std::cout << "Loading player saved keybindings..." << std::endl;
	std::filesystem::path keybindingPath = SAVED_SETTINGS_DIRECTORY / "keybindings.json";
	json::JsonFileWrapper json{ keybindingPath, "keybindings" };
	if (!json.load())
	{
		std::cout << "Could not load " << keybindingPath << std::endl;
		std::cout << "--- Using default settings" << std::endl;
		_keybindings.restoreDefaultKeybindings();
	}
	else
	{
		_keybindings.createFromJson(json.getRoot());
	}
}

void drft::Engine::loadDefaultKeybindings()
{
	std::cout << "Loading default keybindings..." << std::endl;
	std::filesystem::path keybindingPath = DEFAULT_SETTINGS_DIRECTORY / "_default_keybindings.json";
	json::JsonFileWrapper json{ keybindingPath, "keybindings" };
	if (!json.load())
	{
		error_logger << "Error: " << keybindingPath << " could not be loaded." << std::endl;
	}
	else
	{
		_keybindings.createDefaultsFromJson(json.getRoot());
	}
}

void drft::Engine::setupActionMap()
{
	_actionMap.setKeybindings(_keybindings);

	_actionMap.bind("engine", "general", "toggle_fullscreen",	[this]() {toggleFullscreen();});
	_actionMap.bind("engine", "general", "toggle_debug",		[this]() {toggleDebug();});

	// Movement actions
	_actionMap.bind("engine", "gameplay", "move_east",			[this]() {swapToKeyboard();});
	_actionMap.bind("engine", "gameplay", "move_west",			[this]() {swapToKeyboard();});
	_actionMap.bind("engine", "gameplay", "move_north",			[this]() {swapToKeyboard();});
	_actionMap.bind("engine", "gameplay", "move_south",			[this]() {swapToKeyboard();});
	_actionMap.bind("engine", "gameplay", "move_north_east",	[this]() {swapToKeyboard();});
	_actionMap.bind("engine", "gameplay", "move_north_west",	[this]() {swapToKeyboard();});
	_actionMap.bind("engine", "gameplay", "move_south_east",	[this]() {swapToKeyboard();});
	_actionMap.bind("engine", "gameplay", "move_south_west",	[this]() {swapToKeyboard();});
	_actionMap.bind("engine", "gameplay", "wait",				[this]() {swapToKeyboard();});
}

void drft::Engine::registerStates()
{
	_stateStack.registerState<TitleScreenState>(States::Title);
	_stateStack.registerState<MainMenuState>(States::MainMenu);
	_stateStack.registerState<GameState>(States::Game);
	_stateStack.registerState<PauseState>(States::Pause);
	_stateStack.registerState<SettingsState>(States::Settings);
	_stateStack.registerState<KeybindingState>(States::Keybindings);
}

void drft::Engine::handleEvents()
{
	while (const auto event = _window.pollEvent())
	{
		if (auto keypressed = event->getIf<sf::Event::KeyPressed>())
		{
			if (onKeyboardPressed(keypressed->scancode)) continue;
		}
		if (event->is<sf::Event::Closed>())
		{
			shutDown();
		}
		if (event->is<sf::Event::MouseMoved>())
		{
			onMouseMoved();
		}

		if (passEventToGui(event.value())) return;

		passEventToStates(event.value());
	}
}

void drft::Engine::update()
{
	_stateStack.update();
}

void drft::Engine::render(const float)
{
	_window.clear();
	_stateStack.render(_window);
	_gui.draw();

	if (_showDebug)
	{
		// TODO: re-implement a debug display
	}
			
	_window.display();
}

void drft::Engine::shutDown()
{
	std::cout << "Closing Engine" << std::endl;

	saveKeybindings();

	_window.close();
}

void drft::Engine::onMouseMoved()
{
	if (_controlsContext.navigation == NavigationType::Keyboard)
	{
		swapToMouse();
	}
}

bool drft::Engine::onKeyboardPressed(sf::Keyboard::Scancode scancode)
{
	ModifiedInput input = KeybindingUtils::getModifiedInput(scancode);

	if (_actionMap.call("engine", "general", input)) return true;

	_actionMap.call("engine", "gameplay", input);

	return false;
}

bool drft::Engine::passEventToGui(sf::Event event)
{
	return _gui.handleEvent(event);
}

bool drft::Engine::passEventToStates(sf::Event event)
{
	return _stateStack.handleEvent(event);
}

void drft::Engine::swapToMouse()
{
	_window.setMouseCursorVisible(true);

	_gui.handleEvent(sf::Event::MouseEntered{});
	_gui.unfocusAllWidgets();
	_controlsContext.navigation = NavigationType::Mouse;
}

void drft::Engine::swapToKeyboard()
{
	_window.setMouseCursorVisible(false);

	_gui.handleEvent(sf::Event::MouseLeft{});
	auto container = _gui.getContainer();
	for (auto&& child : container->getWidgets())
	{
		if (!child->isVisible()) continue;
		child->setFocused(true);
		break;
	}
	_controlsContext.navigation = NavigationType::Keyboard;
}

void drft::Engine::toggleFullscreen()
{
	if (_isFullScreen)
	{
		_window.create(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Drifter Engine");
		_window.setVerticalSyncEnabled(false);
		_isFullScreen = false;
		std::cout << "Window set to windowed mode" << std::endl;
	}
	else
	{
		_window.create(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Drifter Engine", sf::State::Fullscreen);
		_window.setVerticalSyncEnabled(true);
		_isFullScreen = true;
		std::cout << "Window set to fullscreen mode" << std::endl;
	}
}

void drft::Engine::toggleDebug()
{
	_showDebug = !_showDebug;
}

void drft::Engine::saveKeybindings()
{
	std::cout << "Saving keybindings..." << std::endl;
	std::filesystem::path keybindingPath = SAVED_SETTINGS_DIRECTORY / "keybindings.json";
	
	json::JsonFileWrapper keybindingFile{ keybindingPath, "keybindings"};
	if (keybindingFile.create())
	{
		auto& val = keybindingFile.getRoot();
		_keybindings.saveToJson(keybindingFile.getRoot(), keybindingFile.getAllocator());
		keybindingFile.save();
	}
}
