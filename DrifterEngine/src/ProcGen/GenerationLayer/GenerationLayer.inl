#include <ProcGen/GenerationLayer/GenerationLayer.h>
#include <ProcGen/GenerationLayer/GenerationLayerManager.h>
#include <ProcGen/GenerationLayer/Layers/EntityLayer.h>
#include <ProcGen/GenerationLayer/AbstractChunk.h>


template<typename Layer, typename Chunk>
inline GenerationState GenerationLayer<Layer, Chunk>::generate(GenerationContext&& context)
{
	const auto chunks = getChunkPointsInsideArea(context.area);

	const bool chunksLoaded = checkIfChunksLoaded(chunks);
	if (chunksLoaded) return GenerationState::Complete;

	const bool dependenciesReady = generateDependencies(context);
	if (!dependenciesReady) return GenerationState::Generating;

	const bool chunksReady = generateChunks(chunks, context);
	if (!chunksReady) return GenerationState::Generating;
	
	return GenerationState::Complete;
}

template<typename Layer, typename Chunk>
inline void GenerationLayer<Layer, Chunk>::addDependency(GenerationLayerDependency dependency)
{
	_dependencies.push_back(std::move(dependency));
}

template<typename Layer, typename Chunk>
inline Chunk* GenerationLayer<Layer, Chunk>::tryGetChunk(sf::Vector2i tilePosition)
{
	const auto chunkPosition = toChunkPosition(tilePosition);
	if (_chunks.contains(chunkPosition))
	{
		return &_chunks[chunkPosition];
	}
	return nullptr;
}

template<typename Layer, typename Chunk>
inline sf::Vector2i GenerationLayer<Layer, Chunk>::toChunkPosition(sf::Vector2i tilePosition)
{
	sf::Vector2i result;
	result.x = tilePosition.x / dimensions().x;
	result.y = tilePosition.y / dimensions().y;
	return result;
}

template<typename Layer, typename Chunk>
inline sf::Vector2i GenerationLayer<Layer, Chunk>::toChunkLocalPosition(sf::Vector2i tilePosition)
{
	sf::Vector2i result;
	result.x = tilePosition.x % dimensions().x;
	result.y = tilePosition.y % dimensions().y;
	return result;
}

template<typename Layer, typename Chunk>
inline std::vector<sf::Vector2i> GenerationLayer<Layer, Chunk>::getChunkPointsInsideArea(sf::IntRect area)
{
	std::vector<sf::Vector2i> result;
	sf::Vector2i top_left_point = toChunkPosition({ area.left, area.top});
	sf::Vector2i bottom_right_point = toChunkPosition({ area.left + area.width, area.top + area.height });
	for (int y = top_left_point.y; y <= bottom_right_point.y; ++y)
	{
		for (int x = top_left_point.x; x <= bottom_right_point.x; ++x)
		{
			result.push_back({ x, y });
		}
	}
	return result;
}

template<typename Layer, typename Chunk>
inline bool GenerationLayer<Layer, Chunk>::checkIfChunksLoaded(const std::vector<sf::Vector2i>& chunks) const
{
	for (auto&& point : chunks)
	{
		if (!_chunks.contains(point))
		{
			return false;
		}

		const AbstractChunk* chunk = static_cast<const AbstractChunk*>(&_chunks.at(point));
		if (!chunk->isGenerated())
		{
			return false;
		}
	}
	return false;
}

template<typename Layer, typename Chunk>
inline bool GenerationLayer<Layer, Chunk>::generateDependencies(GenerationContext& context)
{
	bool result = true;
	for (auto&& [id, padding] : _dependencies)
	{
		auto& layer = context.layers.get(id);
		sf::IntRect paddedArea = context.area;
		paddedArea.left -= padding.x / 2;
		paddedArea.top -= padding.y / 2;
		paddedArea.width += padding.x / 2;
		paddedArea.height += padding.x / 2;
		GenerationState state = layer.generate({ .area = paddedArea, .layers = context.layers });
		if (state == GenerationState::Generating)
		{
			result = false;
		}
	}
	return result;
}

template<typename Layer, typename Chunk>
inline bool GenerationLayer<Layer, Chunk>::generateChunks(const std::vector<sf::Vector2i>& chunks, GenerationContext& context)
{
	bool result = true;
	for (auto&& point : chunks)
	{
		if (!_chunks.contains(point))
		{
			_chunks.emplace(point, Chunk{});
		}
			
		AbstractChunk* chunk = static_cast<AbstractChunk*>(&_chunks.at(point));
		GenerationState state = chunk->generate();
		if (state == GenerationState::Generating)
		{
			result = false;
		}
	}
	return result;
}
