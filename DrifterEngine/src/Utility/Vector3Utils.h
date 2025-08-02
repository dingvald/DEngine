#pragma once

namespace drft
{
	class Vector3Utils
	{
	public:
		template<typename T>
		static std::string toString(const sf::Vector3<T>& vec)
		{
			return std::format("({}, {}, {})", vec.x, vec.y, vec.z);
		}
		template<typename T>
		static std::string toString(const sf::Vector2<T>& vec)
		{
			return std::format("({}, {})", vec.x, vec.y);
		}
		static bool isNear(sf::Vector3f v1, sf::Vector3f v2, float epsilon = 0.001f);
		static sf::Vector3f normalize(sf::Vector3f v);
		static float magnitude(sf::Vector3f v);
		static sf::Vector3f floor(const sf::Vector3f& v);
		static sf::Vector3f floorToMultiple(const sf::Vector3f& v, float multiple);
		static sf::Vector2f floor(const sf::Vector2f v);
	};
	
}