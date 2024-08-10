#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <EnTT/core/fwd.hpp>
#include <vector>
#include <functional>
#include <memory>
#include <Utility/stdHashing.h>

inline sf::IntRect addPaddingToArea(sf::IntRect area, sf::Vector2i padding)
{
	sf::IntRect result = area;
	result.left -= padding.x / 2;
	result.top -= padding.y / 2;
	result.width += padding.x / 2;
	result.height += padding.x / 2;
	return result;
}

enum class GenerationState
{
	Generating,
	Complete,
	Failed
};

class GenerationLayerManager;

struct GenerationContext
{
	sf::IntRect area;
	GenerationLayerManager& layers;
};

struct GenerationLayerDependency
{
	entt::id_type layerID;
	sf::Vector2i padding;
};

class AbstractLayer
{
public:
	virtual GenerationState generate(GenerationContext&& context) = 0;
	virtual bool isLoadedInArea(sf::IntRect area) const = 0;
};

class AbstractChunk
{
public:
	virtual GenerationState generate() = 0;
	virtual void destroy() = 0;
	virtual bool isGenerated() const = 0;
	virtual void setIsGenerated(bool value) = 0;
};

class GenerationLayerManager
{
public:
	template<typename T>
	GenerationState generate(sf::IntRect area)
	{
		static_assert(std::is_convertible<T*, AbstractLayer*>::value, "Type must inherit from AbstractLayer");
		entt::id_type id = entt::type_index<T>::value();
		return _layers.at(id)->generate({ .area = area, .layers = *this });
	}
	GenerationState generate(entt::id_type id, sf::IntRect area)
	{
		if (_layers.contains(id))
		{
			return _layers.at(id)->generate({ .area = area, .layers = *this });
		}
		return GenerationState::Failed;
	}

	template<typename T>
	void add()
	{
		static_assert(std::is_convertible<T*, AbstractLayer*>::value, "Type must inherit from AbstractLayer");
		entt::id_type id = entt::type_index<T>::value();
		_layers.emplace(id, std::make_unique<T>());
	}

	template<typename T>
	T& get()
	{
		static_assert(std::is_convertible<T*, AbstractLayer*>::value, "Type must inherit from AbstractLayer");
		entt::id_type id = entt::type_index<T>::value();
		return *static_cast<T*>(_layers.at(id).get());
	}
	AbstractLayer& get(entt::id_type id)
	{
		return *(_layers.at(id).get());
	}

	template<typename T>
	bool has()
	{
		static_assert(std::is_convertible<T*, AbstractLayer*>::value, "Type must inherit from AbstractLayer");
		entt::id_type id = entt::type_index<T>::value();
		return _layers.contains(id);
	}
	bool has(entt::id_type id)
	{
		return _layers.contains(id);
	}

private:
	using AbstractLayerPtr = std::unique_ptr<AbstractLayer>;
	std::unordered_map<entt::id_type, AbstractLayerPtr> _layers;
};

template<typename LayerType, typename ChunkType>
class GenerationChunk : public AbstractChunk
{
public:
	GenerationChunk(sf::IntRect bounds, LayerType& layer, GenerationLayerManager& layers)
		: _layer(layer)
		, _bounds(bounds)
		, _layerManager(layers)
	{}

	virtual GenerationState generate() override { return GenerationState::Complete; };
	virtual void destroy() override {};
	virtual bool isGenerated() const override final { return _isGenerated; }
	void setIsGenerated(bool value) override final { _isGenerated = value; }

protected:
	template<typename T>
	T* tryGetDependency()
	{
		if constexpr (!std::is_convertible<T*, AbstractLayer*>::value)
		{
			static_assert(false, "Type must be derived from AbstractLayer");
		}
		else
		{
			const entt::id_type typeId = entt::type_index<T>::value();
			const std::vector<GenerationLayerDependency>& layerDeps = _layer.getDependencies();
			for (auto&& [id, padding] : layerDeps)
			{
				if (id != typeId) continue;
				auto& layer = _layerManager.get(id);
				if (layer.isLoadedInArea(addPaddingToArea(_bounds, padding)))
				{
					return &static_cast<T&>(layer);
				}
			}
		}
		return nullptr;
	}
	sf::IntRect bounds() const
	{
		return _bounds;
	}

private:
	LayerType& _layer;
	GenerationLayerManager& _layerManager;
	sf::IntRect _bounds;
	bool _isGenerated = false;
};

template<typename LayerType, typename ChunkType>
class GenerationLayer : public AbstractLayer
{
public:
	GenerationState generate(GenerationContext&& context) override final
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
	bool isLoadedInArea(sf::IntRect area) const override final
	{
		const auto pointsInArea = getChunkPointsInsideArea(area);
		return checkIfChunksLoaded(pointsInArea);
	}
	const std::vector<GenerationLayerDependency>& getDependencies()
	{
		return _dependencies;
	}

protected:
	template<typename T>
	void addDependency(sf::Vector2i padding)
	{
		GenerationLayerDependency dependency;
		dependency.layerID = entt::type_index<T>::value();
		dependency.padding = padding;
		_dependencies.emplace_back(std::move(dependency));
	}
	void setChunkDimensions(sf::Vector2i dimensions)
	{
		_chunkDimensions = dimensions;
	}
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
		if (_chunks.contains(chunkPosition))
		{
			return &_chunks[chunkPosition];
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
	bool checkIfChunksLoaded(const std::vector<sf::Vector2i>& chunks) const
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
		return true;
	}
	bool generateDependencies(GenerationContext& context)
	{
		bool result = true;
		for (auto&& [id, padding] : _dependencies)
		{
			auto& layer = context.layers.get(id);
			const auto paddedArea = addPaddingToArea(context.area, padding);
			GenerationState state = layer.generate({ .area = paddedArea, .layers = context.layers});
			if (state == GenerationState::Generating)
			{
				result = false;
			}
		}
		return result;
	}
	bool generateChunks(const std::vector<sf::Vector2i>& chunks, GenerationContext& context)
	{
		bool result = true;
		for (auto&& point : chunks)
		{
			if (!_chunks.contains(point))
			{
				ChunkType chunk = ChunkType{ sf::IntRect{toTilePosition(point), _chunkDimensions}, *static_cast<LayerType*>(this), context.layers };
				_chunks.emplace(point, std::move(chunk));
			}

			AbstractChunk* chunk = static_cast<AbstractChunk*>(&_chunks.at(point));
			GenerationState state = chunk->generate();
			if (state == GenerationState::Generating)
			{
				result = false;
			}
			else if (state == GenerationState::Complete)
			{
				chunk->setIsGenerated(true);
			}
		}
		return result;
	}

private:
	entt::dense_map<sf::Vector2i, ChunkType> _chunks;
	std::vector<GenerationLayerDependency> _dependencies;
	sf::Vector2i _chunkDimensions;
};