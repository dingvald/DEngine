#pragma once
#include "Systems/System.h"
#include "Spatial/VirtualChunk.h"
#include "Utility/stdHashing.h"

namespace drft::system
{
	namespace
	{
		enum ProcessType
		{
			BUILD,
			SAVE,
			LOAD
		};
	}

	class ChunkManager : public System
	{
	public:
		using System::System;

		virtual void onUpdate(const float dt) override;
		virtual void save(cereal::JSONOutputArchive& oarchive);

	private:
		void updateChunkStates(sf::Vector2i aroundNewPosition);
		void cleanUpChunks(sf::Vector2i newPosition);
		void process(std::queue<sf::Vector2i>& chunkQueue, ProcessType type);

	private:
		std::unordered_map<sf::Vector2i, spatial::VirtualChunk> _chunks;
		sf::Vector2i _currentPosition = { 0, 0 };

		std::queue<sf::Vector2i> _toBuild;
		std::queue<sf::Vector2i> _toLoad;
		std::queue<sf::Vector2i> _toSave;
	};
}



