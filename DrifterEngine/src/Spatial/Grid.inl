#pragma once
#include "Grid.h"
#include <algorithm>

namespace drft::spatial
{
	template<typename T>
	Grid<T>::Grid(int width, int height)
		: _width(width), _height(height)
	{
		_cells.resize(_width * _height);
	}

	template<typename T>
	inline Grid<T>::Grid(int width, int height, std::initializer_list<T> list)
		: _width(width)
		, _height(height)
	{
		for (auto element : list)
		{
			_cells.emplace_back(element);
		}
	}

	template<typename T>
	inline void Grid<T>::resize(int width, int height)
	{
		_width = width;
		_height = height;
		_cells.resize(width * height);
	}

	template<typename T>
	inline void Grid<T>::fill(T val)
	{
		std::fill(_cells.begin(), _cells.end(), val);
	}

	template<typename T>
	inline bool Grid<T>::contains(int x, int y, T val)
	{
		return (_cells.at(y * _width + x) == val);
	}

	template<typename T>
	inline bool Grid<T>::contains(sf::IntRect rect, T val)
	{
		int miny = std::min(0, rect.top);
		int minx = std::min(0, rect.left);
		int maxy = std::max(_height, rect.top + rect.height);
		int maxx = std::max(_width, rect.left + rect.width);
		for (int y = miny; y < maxy; ++y)
		{
			for (int x = minx; x < maxx; ++x)
			{
				if (_cells.at(y * _width + x) == val)
					return true;
			}
		}
		return false;
	}

	template<typename T>
	inline T& Grid<T>::at(int x, int y)
	{
		return _cells.at(y * _width + x);
	}

	template<typename T>
	inline const T& Grid<T>::at(int x, int y) const
	{
		return _cells.at(y * _width + x);
	}

	template<typename T>
	inline int Grid<T>::width() const
	{
		return _width;
	}

	template<typename T>
	inline int Grid<T>::height() const
	{
		return _height;
	}

	template<typename T>
	inline void Grid<T>::clear()
	{
		_cells = std::vector<T>(_width * _height);
	}

}