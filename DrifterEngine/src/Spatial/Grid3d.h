#pragma once
#include <vector>
#include <functional>
#include <SFML/System/Vector3.hpp>

namespace drft::spatial
{
	template<typename T>
	class Grid3d
	{
	public:
		Grid3d() = default;
		Grid3d(sf::Vector3i dimensions);

		void resize(sf::Vector3i dimensions);
		void fill(T val);
		void fill(std::function<T(sf::Vector3i)> fillFunc);

		bool contains(sf::Vector3i) const;

		T& at(sf::Vector3i position);
		const T& at(sf::Vector3i position) const;

		int width() const;
		int height() const;
		int depth() const;

		void clear();

	private:
		std::vector<T> _cells;
		int _width;
		int _height;
		int _depth;
	};

} // namespace drft

#include "Grid3d.inl"