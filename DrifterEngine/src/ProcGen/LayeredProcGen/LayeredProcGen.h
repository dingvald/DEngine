#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <EnTT/core/fwd.hpp>
#include <vector>
#include <functional>
#include <memory>
#include <Spatial/Helpers.h>
#include <Utility/stdHashing.h>
#include <Utility/StandardErrorLogger.h>


enum class GenerationState
{
	Generating,
	Complete,
	Failed
};

inline GenerationState combinedState(std::vector<GenerationState> states)
{
	for (auto&& state : states)
	{
		if (state == GenerationState::Failed || state == GenerationState::Generating)
		{
			return state;
		}
	}
	return GenerationState::Complete;
}

class GenerationLayerManager;

class IGetValueAt
{
public:
	virtual double getValueAt(sf::Vector2i position) = 0;
};

namespace details
{
	struct GenerationContext
	{
		sf::IntRect area;
		GenerationLayerManager& layers;
		int desiredLevel;
		unsigned int seed;
	};

	class AbstractLayer
	{
	public:
		virtual GenerationState generate(GenerationContext&& context) = 0;
	};

	class AbstractChunk
	{
	public:
		virtual GenerationState doGenerate(int level) = 0;
		virtual bool isGenerated() = 0;
	};

	class AbstractOnDemandLayer : public AbstractLayer
	{
		virtual GenerationState generate(GenerationContext&& context) = 0;
	};
}

template<typename T>
struct FutureLayer
{
public:
	bool isReady() const
	{
		return getState() == GenerationState::Complete;
	}
	GenerationState getState() const
	{
		if (_state == GenerationState::Complete)
		{
			if (!_instance)
			{
				error_logger << "Layer id cannot be converted to " << typeid(T).name() << std::endl;
				return GenerationState::Failed;
			}
		}
		return _state;
	}
	T& unwrap()
	{
		if (!_instance) throw std::exception("Trying to unwrap nullptr future layer");
		return *_instance;
	}

private:
	friend class GenerationLayerManager;
	GenerationState _state;
	T* _instance;
};

class GenerationLayerManager
{
public:
	GenerationLayerManager(unsigned int seed)
		: _globalSeed(seed)
	{}

	template<typename T>
	FutureLayer<T> generate(sf::IntRect area, int level = 0)
	{
		static_assert(std::is_convertible<T*, details::AbstractLayer*>::value, "Type must inherit from AbstractLayer");
		entt::id_type type = entt::type_index<T>::value();
		FutureLayer<T> result;
		result._instance = nullptr;
		result._state = _typedLayers.at(type)->generate({ .area = area, .layers = *this, .desiredLevel = level, .seed = _globalSeed });
		if (result._state == GenerationState::Complete)
		{
			result._instance = static_cast<T*>(_typedLayers.at(type).get());
		}
		return result;
	}
	template<typename T>
	FutureLayer<T> generate(entt::id_type id, sf::IntRect area, int level = 0)
	{
		FutureLayer<T> result;
		result._instance = nullptr;
		result._state = _namedLayers.at(id)->generate({ .area = area, .layers = *this, .desiredLevel = level, .seed = _globalSeed });
		if (result._state == GenerationState::Complete)
		{
			result._instance = dynamic_cast<T*>(_namedLayers.at(id).get());
		}
		return result;
	}

	template<typename T>
	void add(std::unique_ptr<T> layer)
	{
		static_assert(std::is_convertible<T*, details::AbstractLayer*>::value, "Type must inherit from AbstractLayer");
		entt::id_type type = entt::type_index<T>::value();
		_typedLayers.emplace(type, std::move(layer));
	}

	template<typename T>
	void add(std::unique_ptr<T> layer, entt::id_type id)
	{
		static_assert(std::is_convertible<T*, details::AbstractLayer*>::value, "Type must inherit from AbstractLayer");
		_namedLayers.emplace(id, std::move(layer));
	}

private:
	using AbstractLayerPtr = std::unique_ptr<details::AbstractLayer>;
	using LayerIdMap = std::unordered_map<entt::id_type, AbstractLayerPtr>;
	LayerIdMap _typedLayers;
	LayerIdMap _namedLayers;
	unsigned int _globalSeed;
};

template<typename LayerType, typename ChunkType>
class GenerationChunk : public details::AbstractChunk
{
public:
	GenerationChunk(sf::Vector2i index, sf::IntRect bounds, LayerType& layer, GenerationLayerManager& layers, unsigned int globalSeed)
		: _index(index)
		, _bounds(bounds)
		, _layer(layer)
		, _layerManager(layers)
	{
		_localSeed = globalSeed + std::hash<sf::Vector2i>{}(sf::Vector2i{ _bounds.left, _bounds.top });
		_globalSeed = globalSeed;
	}

	virtual GenerationState doGenerate(int level) override final 
	{ 
		level = std::min(level, numLevels());
		level = level == 0 ? numLevels() : level; // default level "0" generates all layers

		if (_currentLevel > level) return GenerationState::Complete;

		GenerationState state = this->generate(_currentLevel);
		if (state == GenerationState::Complete)
		{
			_currentLevel++;
			if (_currentLevel <= level)
			{
				state = GenerationState::Generating;
			}
		}
		return state; 
	};
	virtual bool isGenerated() override final { return _currentLevel > numLevels(); }
	
protected:
	virtual GenerationState generate(int level) { return GenerationState::Complete; }
	virtual int numLevels() { return 1; }
	template<typename T>
	FutureLayer<T> generateDependency(entt::id_type id, sf::IntRect area, int level = 0)
	{
		return _layerManager.generate<T>(id, area, level);
	}
	template<typename T>
	FutureLayer<T> generateDependency(sf::IntRect area, int level = 0)
	{
		return _layerManager.generate<T>(area, level);
	}
	sf::IntRect addPaddingToBounds(sf::Vector2i padding)
	{
		sf::IntRect result = _bounds;
		result.left -= padding.x;
		result.top -= padding.y;
		result.width += 2*padding.x;
		result.height += 2*padding.y;
		return result;
	}
	void forEachPointInBounds(std::function<void(sf::Vector2i)> func)
	{
		for (int y = _bounds.top; y < _bounds.top + _bounds.height; y++)
		{
			for (int x = _bounds.left; x < _bounds.left + _bounds.width; x++)
			{
				func(sf::Vector2i{ x, y });
			}
		}
	}
	unsigned int getLocalSeed() const
	{
		return _localSeed;
	}
	unsigned int getGlobalSeed() const
	{
		return _globalSeed;
	}

protected:
	sf::Vector2i _index;
	sf::IntRect _bounds;
	LayerType& _layer;
	GenerationLayerManager& _layerManager;

private:
	unsigned int _localSeed;
	unsigned int _globalSeed;
	int _currentLevel = 1;
};

template<typename LayerType, typename ChunkType>
class GenerationLayer : public details::AbstractLayer
{
public:
	GenerationLayer(sf::Vector2i chunkDimensions)
		: _chunkDimensions(chunkDimensions)
	{}

	GenerationState generateNeighborChunks(sf::Vector2i chunkCoordinate, details::GenerationContext&& context)
	{
		auto neighbors = drft::spatial::getAdjacentPoints(chunkCoordinate);
		bool result = generateChunks(neighbors, context);
		if (!result) return GenerationState::Generating;
		return GenerationState::Complete;
	}
protected:
	sf::Vector2i getChunkDimensions() const
	{
		return _chunkDimensions;
	}

	sf::Vector2i toChunkPosition(sf::Vector2i tilePosition) const
	{
		sf::Vector2i result;
		result.x = tilePosition.x / _chunkDimensions.x;
		result.y = tilePosition.y / _chunkDimensions.y;
		return result;
	}
	sf::Vector2i toChunkLocalPosition(sf::Vector2i tilePosition) const
	{
		sf::Vector2i result;
		result.x = tilePosition.x % _chunkDimensions.x;
		result.y = tilePosition.y % _chunkDimensions.y;
		return result;
	}
	sf::Vector2i toTilePosition(sf::Vector2i chunkPosition) const
	{
		sf::Vector2i result;
		result.x = chunkPosition.x * _chunkDimensions.x;
		result.y = chunkPosition.y * _chunkDimensions.y;
		return result;
	}
	std::vector<sf::Vector2i> getChunkPointsInsideArea(sf::IntRect area) const
	{
		std::vector<sf::Vector2i> result;
		sf::Vector2i top_left_point = toChunkPosition({ area.left, area.top });
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

	ChunkType* tryGetChunk(sf::Vector2i tilePosition)
	{
		const auto chunkPosition = toChunkPosition(tilePosition);
		if (_chunks.contains(chunkPosition) && _chunks.at(chunkPosition).isGenerated())
		{
			return &_chunks.at(chunkPosition);
		}
		return nullptr;
	}
	void forEachLoadedChunk(std::function<void(ChunkType&)> func)
	{
		for (auto&& chunk : _chunks)
		{
			func(chunk);
		}
	}
	void forEachLoadedChunkInArea(sf::IntRect area, std::function<void(ChunkType&)> func)
	{
		auto chunkPoints = getChunkPointsInsideArea(area);
		for (auto&& chunkPoint : chunkPoints)
		{
			if (!_chunks.contains(chunkPoint)) continue;
			ChunkType& chunk = _chunks.at(chunkPoint);
			func(chunk);
		}
	}

private:
	GenerationState generate(details::GenerationContext&& context) override final
	{
		const auto chunks = getChunkPointsInsideArea(context.area);
		const bool chunksReady = generateChunks(chunks, context);
		if (!chunksReady) return GenerationState::Generating;

		return GenerationState::Complete;
	}
	bool generateChunks(const std::vector<sf::Vector2i>& chunks, details::GenerationContext& context)
	{
		bool result = true;
		for (auto&& point : chunks)
		{
			if (!_chunks.contains(point))
			{
				ChunkType chunk = ChunkType{
					point,
					sf::IntRect{toTilePosition(point), _chunkDimensions}, 
					*static_cast<LayerType*>(this), 
					context.layers, 
					context.seed 
				};
				_chunks.emplace(point, std::move(chunk));
			}

			details::AbstractChunk* chunk = static_cast<details::AbstractChunk*>(&_chunks.at(point));
			GenerationState state = chunk->doGenerate(context.desiredLevel);
			if (state == GenerationState::Generating)
			{
				result = false;
			}
		}
		return result;
	}

private:
	entt::dense_map<sf::Vector2i, ChunkType> _chunks;
	sf::Vector2i _chunkDimensions;
};

class OnDemandLayer : public details::AbstractOnDemandLayer, public IGetValueAt
{
public:
	virtual double getValueAt(sf::Vector2i tilePosition) = 0;
	virtual GenerationState generate(details::GenerationContext&& context) override final
	{
		_globalSeed = context.seed;
		return GenerationState::Complete;
	}

protected:
	unsigned int getGlobalSeed() const
	{
		return _globalSeed;
	}

private:
	unsigned int _globalSeed;
};