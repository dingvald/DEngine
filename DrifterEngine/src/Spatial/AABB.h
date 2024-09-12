#pragma once
#include <SFML/System/Vector3.hpp>
#include <Utility/Math.h>

namespace drft::spatial
{
	template<typename T>
	struct AABB
	{
		AABB();
		AABB(const sf::Vector3<T>& position, const sf::Vector3<T>& size);

		bool contains(const sf::Vector3<T>& point) const;
		bool intersects(const AABB<T>& aabb) const;
		sf::Vector3i dimensions() const;

		sf::Vector3<T> min; // top-back-left corner
		sf::Vector3<T> max; // bottom-front-right corner
	};

	template<typename T>
	inline AABB<T>::AABB()
		: min(0,0,0)
		, max(0,0,0)
	{}

	template<typename T>
	inline AABB<T>::AABB(const sf::Vector3<T>& position, const sf::Vector3<T>& size)
		: min(position)
		, max(position + size)
	{}

	template<typename T>
	inline bool AABB<T>::contains(const sf::Vector3<T>& point) const
	{
		return min.x < point.x 
			&& min.y < point.y 
			&& min.z < point.z
			&& max.x > point.x
			&& max.y > point.y
			&& max.z > point.z;
	}

	template<typename T>
	inline bool AABB<T>::intersects(const AABB<T>& aabb) const
	{
		bool x_intersection = math::isOverlapping(this->min.x, this->max.x, aabb.min.x, aabb.max.x);
		bool y_intersection = math::isOverlapping(this->min.y, this->max.y, aabb.min.y, aabb.max.y);
		bool z_intersection = math::isOverlapping(this->min.z, this->max.z, aabb.min.z, aabb.max.z);
		return x_intersection && y_intersection && z_intersection;
	}

	template<typename T>
	inline sf::Vector3i AABB<T>::dimensions() const
	{
		return max - min;
	}
}