#pragma once
#include <SFML/Graphics/Color.hpp>

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, sf::Color& color)
	{
		archive(color.r, color.g, color.b, color.a);
	}
}