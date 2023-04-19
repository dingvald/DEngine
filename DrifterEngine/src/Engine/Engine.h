#pragma once
#include "Utility/ResourceHolder.h"
#include "StateStack.h"

namespace drft
{
	class Engine
	{
	public:
		Engine();
		void run();

	private:
		void initialize();
		void loadResources();
		void registerStates();
		void handleEvents();
		void update(const float dt);
		void render(const float dt);
		void shutDown();

	private:
		sf::RenderWindow _window;
		entt::registry _registry;
		ResourceHolder<sf::Texture, std::string> _textures;
		ResourceHolder<sf::Font, std::string> _fonts;
		StateStack _stateStack{ StateContext{_window, _registry, _textures, _fonts} };

		bool _showDebug;
	};

} // namespace drft






