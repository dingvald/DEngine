#pragma once

#include <unordered_map>
#include <memory>
#include "Grid2d.h"

namespace drft::spatial
{
	template<typename T>
	class AutoGrid
	{
	public:
		AutoGrid(int subchunk_width = 64, int subchunk_height = 64);

		void fill(T val, int left, int top, int width, int height);
		T& at(int x, int y);
		const T& at(int x, int y) const;

		// Removes the chunk at the given coordinates
		void discard(sf::Vector2i coordinate);

	private:
		using ChunkPtr = std::unique_ptr<spatial::Grid2d<T>>;
		sf::Vector2i toSubchunk(int x, int y) const;
		int toLocalX(int x) const;
		int toLocalY(int y) const;
		Grid2d<T>& getOrCreateChunk(sf::Vector2i key) const;

	private:
		int _subchunkWidth;
		int _subchunkHeight;
		mutable std::unordered_map<sf::Vector2i, ChunkPtr > _subchunks;
	};
}

#include "AutoGrid.inl"