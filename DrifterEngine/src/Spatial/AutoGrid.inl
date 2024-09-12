#pragma once
#include "AutoGrid.h"

using namespace drft::spatial;

template<typename T>
inline AutoGrid<T>::AutoGrid(int subchunk_width, int subchunk_height)
	: _subchunkWidth(subchunk_width)
	, _subchunkHeight(subchunk_height)
{}

template<typename T>
inline void drft::spatial::AutoGrid<T>::fill(T val, int left, int top, int width, int height)
{
	for (int y = top; y < top + height; ++y)
	{
		for (int x = left; x < left + width; ++x)
		{
			this->at(x, y) = val;
		}
	}
}

template<typename T>
inline T& drft::spatial::AutoGrid<T>::at(int x, int y)
{
	auto chunk_coord = toSubchunk(x, y);
	auto& subchunk = getOrCreateChunk(chunk_coord);
	return subchunk.at(toLocalX(x), toLocalY(y));
}

template<typename T>
inline const T& drft::spatial::AutoGrid<T>::at(int x, int y) const
{
	auto chunk_coord = toSubchunk(x, y);
	auto& subchunk = getOrCreateChunk(chunk_coord);
	return subchunk.at(toLocalX(x), toLocalY(y));
}

template<typename T>
inline void drft::spatial::AutoGrid<T>::discard(sf::Vector2i coordinate)
{
	_subchunks.erase(coordinate);
}

template<typename T>
inline sf::Vector2i drft::spatial::AutoGrid<T>::toSubchunk(int x, int y) const
{
	int xChunk = x / _subchunkWidth;
	if (x < 0)
	{
		xChunk = (x + 1) / _subchunkWidth;
		--xChunk;
	}

	int yChunk = y / _subchunkHeight;
	if (y < 0)
	{
		yChunk = (y + 1) / _subchunkHeight;
		--yChunk;
	}
	return { xChunk, yChunk };
}

template<typename T>
inline int drft::spatial::AutoGrid<T>::toLocalX(int x) const
{
	int xPos = x % _subchunkWidth;
	if (xPos < 0)
	{
		xPos += _subchunkWidth;
	}
	return xPos;
}

template<typename T>
inline int drft::spatial::AutoGrid<T>::toLocalY(int y) const
{
	int yPos = y % _subchunkHeight;
	if (yPos < 0)
	{
		yPos += _subchunkHeight;
	}
	return yPos;
}

template<typename T>
inline Grid2d<T>& drft::spatial::AutoGrid<T>::getOrCreateChunk(sf::Vector2i key) const
{
	if (!_subchunks.contains(key))
	{
		_subchunks.emplace(key, std::make_unique<spatial::Grid2d<T>>(_subchunkWidth, _subchunkHeight));
	}
	return *_subchunks.at(key);
}
