#pragma once
#include <Spatial/Grid3d.h>

namespace drft::spatial
{
	template<typename T>
	Grid3d<T>::Grid3d(sf::Vector3i dimensions)
		: _width(dimensions.x)
		, _height(dimensions.y)
		, _depth(dimensions.z)
	{
		_cells.resize(_width * _height * _depth);
	}

	template<typename T>
	inline void Grid3d<T>::resize(sf::Vector3i dimensions)
	{
		_width = dimensions.x;
		_height = dimensions.y;
		_depth = dimensions.z;
		_cells.resize(_width * _height * _depth);
	}

	template<typename T>
	inline void Grid3d<T>::fill(T val)
	{
		std::fill(_cells.begin(), _cells.end(), val);
	}

	template<typename T>
	inline void Grid3d<T>::fill(std::function<T(sf::Vector3i)> fillFunc)
	{
		for (int x = 0; x < _width; x++)
		{
			for (int y = 0; y < _height; y++)
			{
				for (int z = 0; z < _depth; z++)
				{
					sf::Vector3i pos = { x, y, z };
					this->at(pos) = fillFunc(pos);
				}
			}
		}
	}

	template<typename T>
	inline bool Grid3d<T>::contains(sf::Vector3i pos) const
	{
		return !(pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= _width || pos.y >= _height || pos.z >= _depth);
	}

	template<typename T>
	inline T& Grid3d<T>::at(sf::Vector3i position)
	{
		return _cells[position.z * _width * _height + position.y * _width + position.x];
	}

	template<typename T>
	inline const T& Grid3d<T>::at(sf::Vector3i position) const
	{
		return _cells[position.z * _width * _height + position.y * _width + position.x];
	}

	template<typename T>
	inline int Grid3d<T>::width() const
	{
		return _width;
	}

	template<typename T>
	inline int Grid3d<T>::height() const
	{
		return _height;
	}

	template<typename T>
	inline int Grid3d<T>::depth() const
	{
		return _depth;
	}

	template<typename T>
	inline void Grid3d<T>::clear()
	{
		_cells = std::vector<T>(_width * _height * _depth);
	}
}