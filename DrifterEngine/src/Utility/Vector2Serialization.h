#pragma once
#include <SFML/System/Vector2.hpp>

namespace cereal
{
	template<class Archive, typename T>
	void serialize(Archive& archive, sf::Vector2<T>& vector)
	{
		archive(vector.x, vector.y);
	}
}