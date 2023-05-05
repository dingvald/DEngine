#pragma once
#include "Grid.h"

namespace drft::spatial
{
	template<typename T>
	Grid<T>::Grid(int width, int height)
		: _width(width), _height(height)
	{
		_cells.resize(_width * _height);
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