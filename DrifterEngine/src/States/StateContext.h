#pragma once
#include "Utility/ResourceHolder.h"
#include "Utility/TextureAtlas.h"
#include <Engine/ControlsContext.h>

class Keybindings;

namespace drft
{
	struct StateContext
	{
		using TextureHolder = ResourceHolder<sf::Texture, std::string>;
		using FontHolder = ResourceHolder<sf::Font, std::string>;

		StateContext(
			sf::RenderWindow& window, 
			entt::registry& registry, 
			TextureAtlas& textures, 
			FontHolder& fonts,
			tgui::Gui& gui,
			const ControlsContext& controls,
			Keybindings& keybindings)
			: window(window)
			, registry(registry)
			, textures(textures)
			, fonts(fonts)
			, gui(gui)
			, controls(controls)
			, keybindings(keybindings)
		{}

		sf::RenderWindow& window;
		entt::registry& registry;
		TextureAtlas& textures;
		FontHolder& fonts;
		tgui::Gui& gui;
		const ControlsContext& controls;
		Keybindings& keybindings;
	};
}


