#pragma once
#include <SFML/System.hpp>
#include <Spatial/Grid3d.h>
#include <unordered_map>
#include <memory>

namespace drft::spatial
{
	template<typename T>
	class AutoGrid3d
	{
	public:
		AutoGrid3d(sf::Vector3i chunkDimension);

		T& at(sf::Vector3i position);
		const T& at(sf::Vector3i position) const;

		// Removes the chunk containing the given position
		void discard(sf::Vector3i position);

	private:
		using ChunkPtr = std::unique_ptr<spatial::Grid3d<T>>;
		sf::Vector3i toSubchunk(sf::Vector3i position) const;
		sf::Vector3i toLocalPosition(sf::Vector3i position) const;
		Grid3d<T>& getOrCreateChunk(sf::Vector3i position) const;

	private:
		sf::Vector3i _chunkDimensions;
		mutable std::unordered_map<sf::Vector3i, ChunkPtr > _subchunks;
	};
}

#include "AutoGrid3d.inl"