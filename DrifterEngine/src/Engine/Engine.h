#pragma once
#include "Utility/ResourceHolder.h"
#include "Utility/TextureAtlas.h"
#include "States/StateIdentifiers.h"
#include "StateStack.h"
#include "Utility/Statistics.h"
#include <Keybindings/Keybindings.h>
#include <Actions/ActionMap.h>
#include <Engine/Debug/DebugDisplay.h>

namespace drft
{
	class Engine
	{
	public:
		Engine();
		void run();

	private:
		void initialize();
		void setWindowIcon();
		void loadResources();
		void loadSavedKeybindings();
		void loadDefaultKeybindings();
		void setupActionMap();
		void registerStates();
		void handleEvents();
		void update();
		void render(const float dt);
		void shutDown();

		void onMouseMoved();
		bool onKeyboardPressed(sf::Keyboard::Scancode scancode);

		bool passEventToGui(sf::Event event);
		bool passEventToStates(sf::Event event);

		void swapToMouse();
		void swapToKeyboard();

		void toggleFullscreen();
		void toggleDebug();

		void saveKeybindings();

	private:
		sf::RenderWindow _window;
		tgui::Gui _gui;
		entt::registry _registry;
		TextureAtlas _textures;
		ResourceHolder<sf::Font, std::string> _fonts;
		ControlsContext _controlsContext = {};
		DebugDisplay _debugDisplay{_window};

		Keybindings _keybindings;
		ActionMap _actionMap{_keybindings};

		StateContext _stateContext = {
			_window,
			_registry,
			_textures,
			_fonts,
			_gui,
			_controlsContext,
			_keybindings,
			_actionMap
		};
		StateStack _stateStack{ _stateContext };

		bool _showDebug;
		bool _isFullScreen = false;
	};

} // namespace drft






