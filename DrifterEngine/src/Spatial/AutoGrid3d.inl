#pragma once
#include "AutoGrid3d.h"

namespace drft::spatial
{
	template<typename T>
	inline AutoGrid3d<T>::AutoGrid3d(sf::Vector3i chunkDimension)
		: _chunkDimensions(chunkDimension)
	{
	}
	template<typename T>
	inline T& AutoGrid3d<T>::at(sf::Vector3i position)
	{
		auto chunkPos = toSubchunk(position);
		auto& chunk = getOrCreateChunk(chunkPos);
		return chunk.at(toLocalPosition(position));
	}
	template<typename T>
	inline const T& AutoGrid3d<T>::at(sf::Vector3i position) const
	{
		auto chunkPos = toSubchunk(position);
		auto& chunk = getOrCreateChunk(chunkPos);
		return chunk.at(toLocalPosition(position));
	}
	template<typename T>
	inline void AutoGrid3d<T>::discard(sf::Vector3i position)
	{
		auto chunk = toSubchunk(position);
		_subchunks.erase(chunk);
	}
	template<typename T>
	inline sf::Vector3i AutoGrid3d<T>::toSubchunk(sf::Vector3i position) const
	{
		sf::Vector3i result = position.componentWiseDiv(_chunkDimensions);
		if (result.x < 0)
		{
			result.x = (position.x + 1) / _chunkDimensions.x;
			--result.x;
		}
		if (result.y < 0)
		{
			result.y = (position.y + 1) / _chunkDimensions.y;
			--result.y;
		}
		if (result.z < 0)
		{
			result.z = (position.z + 1) / _chunkDimensions.z;
			--result.z;
		}
		return result;
	}
	template<typename T>
	inline sf::Vector3i spatial::AutoGrid3d<T>::toLocalPosition(sf::Vector3i position) const
	{
		sf::Vector3i result;
		result.x = position.x % _chunkDimensions.x;
		result.y = position.y % _chunkDimensions.y;
		result.z = position.z % _chunkDimensions.z;
		if (result.x < 0)
		{
			result.x += _chunkDimensions.x;
		}
		if (result.y < 0)
		{
			result.y += _chunkDimensions.y;
		}
		if (result.z < 0)
		{
			result.z += _chunkDimensions.z;
		}
		return result;
	}

	template<typename T>
	inline Grid3d<T>& AutoGrid3d<T>::getOrCreateChunk(sf::Vector3i position) const
	{
		if (!_subchunks.contains(position))
		{
			_subchunks.emplace(position, std::make_unique<spatial::Grid3d<T>>(_chunkDimensions));
		}
		return *_subchunks.at(position);
	}
}

