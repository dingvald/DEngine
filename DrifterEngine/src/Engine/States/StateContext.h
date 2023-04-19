#pragma once
#include "Utility/ResourceHolder.h"

namespace drft
{
	struct StateContext
	{
		using TextureHolder = ResourceHolder<sf::Texture, std::string>;
		using FontHolder = ResourceHolder<sf::Font, std::string>;

		StateContext(sf::RenderWindow& window, entt::registry& registry, TextureHolder& textures, FontHolder& fonts)
			: window(window)
			, registry(registry)
			, textures(textures)
			, fonts(fonts)
		{}

		sf::RenderWindow& window;
		entt::registry& registry;
		TextureHolder& textures;
		FontHolder& fonts;
	};
}


