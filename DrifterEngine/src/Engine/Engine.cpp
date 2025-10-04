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

static const char* WINDOW_TITLE = "Drifter Engine";

static const std::filesystem::path DEFAULT_THEME = "drifter_theme.txt";

static const unsigned int DEFAULT_WINDOW_WIDTH = 2560;
static const unsigned int DEFAULT_WINDOW_HEIGHT = 1440;

static constexpr int UPDATES_PER_FRAME_LIMIT = 10;

static sf::Image ICON_IMAGE;
static const sf::Vector2u ICON_SIZE = { 32u, 32u };


drft::Engine::Engine()
	: _window{ sf::VideoMode{{DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT}}, WINDOW_TITLE }
	, _gui{_window}
{
	initialize();
}

void drft::Engine::run()
{
	sf::Clock clock;
	float lag = 0.0f;

	while (_window.isOpen())
	{
		const float timePassed = clock.restart().asSeconds();
		lag += std::min(timePassed, SECONDS_PER_FRAME * UPDATES_PER_FRAME_LIMIT);

		// Events
		handleEvents();

		DEBUG_DISPLAY_VALUE("dt", std::format("{:.3} ms", timePassed * 1000.f));

		// Update
		update(timePassed);
		while (lag >= SECONDS_PER_FRAME)
		{
			fixedUpdate();
			lag -= SECONDS_PER_FRAME;
		}
		
		// Render
		render(timePassed);

		//
		if (_stateStack.isEmpty()) shutDown();
	}
}

void drft::Engine::initialize()
{
	LOG_MSG("Initializing Engine...");
	LOG_MSG("Working Directory: {}", WORKING_DIRECTORY.string());

	initializeWindow();
	initializeGui();
	initializeDebugDisplay();

	loadResources();
	loadDefaultKeybindings();
	loadSavedKeybindings();
	setupActionMap();
	registerStates();

	_gui.setKeyboardNavigationEnabled(true);
	_window.setMouseCursor(sf::Cursor{ sf::Cursor::Type::Cross });

	_stateStack.pushState(States::Title);
}

void drft::Engine::initializeWindow()
{
	LOG_MSG("Initializing window...");

	// Set window size
	LOG_MSG("Detecting screen resolution");
	sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
	LOG_MSG("Screen Resolution: W: {}, H: {}", videoMode.size.x, videoMode.size.y);
	_window.create(videoMode, WINDOW_TITLE);

	// Set window Icon
	loadWindowIcon();

	LOG_MSG("Window initialized.");
}

void drft::Engine::initializeGui()
{
	_gui.setWindow(_window);
}

void drft::Engine::loadWindowIcon()
{
	const std::filesystem::path iconPath = ICONS_DIRECTORY / "drifter-project-icon.png";
	if (ICON_IMAGE.loadFromFile(iconPath))
	{
		_window.setIcon(ICON_SIZE, ICON_IMAGE.getPixelsPtr());
	}
	else
	{
		LOG_WARNING("Could not set window icon : ");
		LOG_WARNING("File {} not found", iconPath.string());
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
	LOG_MSG("Loading player saved keybindings...");
	std::filesystem::path keybindingPath = SAVED_SETTINGS_DIRECTORY / "keybindings.json";
	json::JsonFileWrapper json{ keybindingPath, "keybindings" };
	if (!json.load())
	{
		LOG_WARNING("Could not load {}", keybindingPath.string());
		LOG_WARNING("Using default settings");
		_keybindings.restoreDefaultKeybindings();
	}
	else
	{
		_keybindings.createFromJson(json.getRoot());
	}
}

void drft::Engine::loadDefaultKeybindings()
{
	LOG_MSG("Loading default keybindings...");
	std::filesystem::path keybindingPath = DEFAULT_SETTINGS_DIRECTORY / "_default_keybindings.json";
	json::JsonFileWrapper json{ keybindingPath, "keybindings" };
	if (!json.load())
	{
		LOG_ERROR("{} could not be loaded", keybindingPath.string());
	}
	else
	{
		_keybindings.createDefaultsFromJson(json.getRoot());
	}
}

void drft::Engine::initializeDebugDisplay()
{
#ifdef DRFT_DEBUG
	auto group = tgui::Group::create();
	_debugDisplay = std::make_unique<DebugDisplay>(group);
	_gui.add(group);
	entt::locator<IDebugDisplay>::emplace<DebugDisplay>(*_debugDisplay);
#endif
}

void drft::Engine::setupActionMap()
{
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
		
		if (passEventToGui(event.value())) continue;
		if (passEventToStates(event.value())) continue;
		if (passEventToDebug(event.value())) continue;
	}
}

void drft::Engine::update(const float dt)
{
	_stateStack.update(dt);
}

void drft::Engine::fixedUpdate()
{
	_stateStack.fixedUpdate();
}

void drft::Engine::render(const float)
{
	if (_debugDisplay) _debugDisplay->moveToFront();

	_window.clear();

	_stateStack.render(_window);
	_gui.draw();
	_stateStack.guiRender(_window);
			
	_window.display();
}

void drft::Engine::shutDown()
{
	LOG_MSG("Closing Engine");

	entt::locator<IDebugDisplay>::reset();

	_stateStack.clearStatesNow();

	saveKeybindings();

	_window.close();
}

void drft::Engine::onMouseMoved()
{
	if (!_controlsContext.isKeyboardControlled()) return;

	// Count mouse movements so transient mouse movements don't activate the cursor
	static int moveCounter = 0; 
	moveCounter++;
	if (moveCounter >= 5)
	{
		moveCounter = 0;
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

bool drft::Engine::passEventToDebug(sf::Event event)
{
	if (_debugDisplay) _debugDisplay->handleEvent(event);
	return false;
}

void drft::Engine::swapToMouse()
{
	_window.setMouseCursorVisible(true);

	_gui.unfocusAllWidgets();
	_controlsContext.setNavigationType(NavigationType::Mouse);
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
	_controlsContext.setNavigationType(NavigationType::Keyboard);
}

void drft::Engine::toggleFullscreen()
{
	sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
	if (_isFullScreen)
	{
		_window.create(videoMode, WINDOW_TITLE);
		_window.setIcon(ICON_SIZE, ICON_IMAGE.getPixelsPtr());
		_window.setVerticalSyncEnabled(false);
		_window.setMouseCursor(sf::Cursor{ sf::Cursor::Type::Cross });
		_isFullScreen = false;
		LOG_MSG("Window set to windowed mode");
	}
	else
	{
		_window.create(videoMode, WINDOW_TITLE, sf::State::Fullscreen);
		_window.setIcon(ICON_SIZE, ICON_IMAGE.getPixelsPtr());
		_window.setVerticalSyncEnabled(true);
		_window.setMouseCursor(sf::Cursor{ sf::Cursor::Type::Cross });
		_isFullScreen = true;
		LOG_MSG("Window set to fullscreen mode");
	}

	// Reset the mouse cursor
	_window.setMouseCursor(sf::Cursor{ sf::Cursor::Type::Cross });
}

void drft::Engine::toggleDebug()
{
	if (!_debugDisplay) return;

	_debugDisplay->setVisible(!_debugDisplay->isVisible());
	_debugDisplay->moveToFront();
}

void drft::Engine::saveKeybindings()
{
	LOG_MSG("Saving keybindings...");
	std::filesystem::path keybindingPath = SAVED_SETTINGS_DIRECTORY / "keybindings.json";
	
	json::JsonFileWrapper keybindingFile{ keybindingPath, "keybindings"};
	if (keybindingFile.create())
	{
		auto& val = keybindingFile.getRoot();
		_keybindings.saveToJson(keybindingFile.getRoot(), keybindingFile.getAllocator());
		keybindingFile.save();
	}
}
