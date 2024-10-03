#pragma once
#include "Utility/ResourceHolder.h"
#include "Utility/TextureAtlas.h"
#include <Engine/ControlsContext.h>

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
			const ControlsContext& controls)
			: window(window)
			, registry(registry)
			, textures(textures)
			, fonts(fonts)
			, gui(gui)
			, controls(controls)
		{}

		sf::RenderWindow& window;
		entt::registry& registry;
		TextureAtlas& textures;
		FontHolder& fonts;
		tgui::Gui& gui;
		const ControlsContext& controls;
	};
}


