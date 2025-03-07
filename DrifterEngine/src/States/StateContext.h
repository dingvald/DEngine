#pragma once
#include "Utility/ResourceHolder.h"
#include "Utility/TextureAtlas.h"
#include <Engine/ControlsContext.h>

class Keybindings;
class ActionMap;

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
			Keybindings& keybindings,
			ActionMap& actionMap)
			: window(window)
			, registry(registry)
			, textures(textures)
			, fonts(fonts)
			, gui(gui)
			, controls(controls)
			, keybindings(keybindings)
			, actions(actionMap)
		{}

		sf::RenderWindow& window;
		entt::registry& registry;
		TextureAtlas& textures;
		FontHolder& fonts;
		tgui::Gui& gui;
		const ControlsContext& controls;
		Keybindings& keybindings;
		ActionMap& actions;
	};
}


