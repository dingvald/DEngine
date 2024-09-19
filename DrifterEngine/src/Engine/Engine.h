#pragma once
#include "Utility/ResourceHolder.h"
#include "Utility/TextureAtlas.h"
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
		void update(const float dt);
		void render(const float dt);
		void shutDown();

	private:
		sf::RenderWindow _window;
		entt::registry _registry;
		TextureAtlas _textures;
		ResourceHolder<sf::Font, std::string> _fonts;
		StateStack _stateStack{ StateContext{_window, _registry, _textures, _fonts} };

		float _dtSinceLastRender = 0.0f;
		float _dtSinceLastFixedUpdate = 0.0f;

		bool _showDebug;
	};

} // namespace drft






