#pragma once
#include "Systems/System.h"
#include "Spatial/VirtualChunk.h"

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
		virtual void init() override;
		virtual void update(const float dt) override;
		void shutdown() override;

	private:
		void updateChunkStates(sf::Vector2i aroundNewPosition);
		void cleanUpChunks(sf::Vector2i newPosition);
		void process(std::queue<sf::Vector2i>& chunkQueue, ProcessType type);

	private:
		std::map<std::pair<int, int>, spatial::VirtualChunk> _chunks;

		const int _activeChunkRadius = 2;
		const int _toSaveRadius = _activeChunkRadius + 1;
		sf::Vector2i _currentPosition = { 0, 0 };

		std::queue<sf::Vector2i> _toBuild;
		std::queue<sf::Vector2i> _toLoad;
		std::queue<sf::Vector2i> _toSave;
	};
}



