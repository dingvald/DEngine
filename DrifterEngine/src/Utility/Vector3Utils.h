#pragma once

namespace drft
{
	class Vector3Utils
	{
	public:
		static bool isNear(sf::Vector3f v1, sf::Vector3f v2, float epsilon = 0.001f);
		static sf::Vector3f normalize(sf::Vector3f v);
		static float magnitude(sf::Vector3f v);
	};
	
}