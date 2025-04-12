#pragma once
#include "Grid2d.h"

namespace drft::spatial
{
	template<typename T>
	Grid2d<T>::Grid2d(int width, int height)
		: _width(width), _height(height)
	{
		_cells.resize(_width * _height);
	}

	template<typename T>
	inline Grid2d<T>::Grid2d(int width, int height, std::initializer_list<T> list)
		: _width(width)
		, _height(height)
	{
		for (auto element : list)
		{
			_cells.emplace_back(element);
		}
	}

	template<typename T>
	inline void Grid2d<T>::resize(int width, int height)
	{
		_width = width;
		_height = height;
		_cells.resize(width * height);
	}

	template<typename T>
	inline void Grid2d<T>::fill(T val)
	{
		std::fill(_cells.begin(), _cells.end(), val);
	}

	template<typename T>
	inline void Grid2d<T>::fill(std::function<T(int, int)> fillFunc)
	{
		for (int y = 0; y < _height; ++y)
		{
			for (int x = 0; x < _width; ++x)
			{
				_cells.at(y * _width + x) = fillFunc(x, y);
			}
		}
	}

	template<typename T>
	inline void Grid2d<T>::fill(T val, int left, int top, int width, int height)
	{
		if (!contains(left, top) || !contains(left + width, top + height))
		{
			throw std::invalid_argument("Argument is out of bounds");
		}
		for (int row = top; row < top + height; ++row)
		{
			for (int col = left; col < left + width; ++col)
			{
				_cells.at(row * _width + col) = val;
			}
		}
	}

	template<typename T>
	inline bool Grid2d<T>::contains(int x, int y) const
	{
		return !(x < 0 || y < 0 || x >= _width || y >= _height);
	}

	template<typename T>
	inline T& Grid2d<T>::at(int x, int y)
	{
		return _cells.at(y * _width + x);
	}

	template<typename T>
	inline const T& Grid2d<T>::at(int x, int y) const
	{
		return _cells.at(y * _width + x);
	}

	template<typename T>
	inline void Grid2d<T>::forEach(std::function<void(int, int, T)> func) const
	{
		for (int y = 0; y < _height; ++y)
		{
			for (int x = 0; x < _width; ++x)
			{
				T val = this->at(x, y);
				func(x, y, val);
			}
		}
	}

	template<typename T>
	inline int Grid2d<T>::width() const
	{
		return _width;
	}

	template<typename T>
	inline int Grid2d<T>::height() const
	{
		return _height;
	}

	template<typename T>
	inline void Grid2d<T>::clear()
	{
		_cells = std::vector<T>(_width * _height);
	}

}