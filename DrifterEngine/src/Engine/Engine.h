#pragma once
#include "Utility/ResourceHolder.h"
#include "Utility/TextureAtlas.h"
#include "States/StateIdentifiers.h"
#include "StateStack.h"
#include "Utility/Statistics.h"

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
		void registerStates();
		void handleEvents();
		void update();
		void render(const float dt);
		void shutDown();

		void handleMouseEvents(sf::Event event);
		void handleKeyboardEvents(sf::Event event);
		void passEventToGui(sf::Event event);
		void passEventToState(sf::Event event);

		void swapToMouse();
		void swapToKeyboard();

		void toggleFullscreen();
		void toggleDebug();

	private:
		sf::RenderWindow _window;
		tgui::Gui _gui;
		entt::registry _registry;
		TextureAtlas _textures;
		ResourceHolder<sf::Font, std::string> _fonts;
		ControlsContext _controlsContext = {};
		StateContext _stateContext = {
			_window,
			_registry,
			_textures,
			_fonts,
			_gui,
			_controlsContext
		};
		StateStack _stateStack{ _stateContext };

		bool _showDebug;
		bool _isFullScreen = false;
	};

} // namespace drft






