#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <EnTT/core/fwd.hpp>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
#include <Spatial/Helpers.h>
#include <Spatial/AABB.h>

#include <Utility/stdHashing.h>
#include <Utility/StandardLogger.h>

namespace drft
{
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
		virtual double getValueAt(sf::Vector3i position) = 0;
	};

	namespace details
	{
		struct GenerationContext
		{
			spatial::AABB<int> volume;
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
			virtual bool isGenerated() const = 0;
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
		FutureLayer<T> generate(spatial::AABB<int> volume, int level = 0)
		{
			static_assert(std::is_convertible<T*, details::AbstractLayer*>::value, "Type must inherit from AbstractLayer");
			entt::id_type type = entt::type_index<T>::value();
			FutureLayer<T> result;
			result._instance = nullptr;
			result._state = _typedLayers.at(type)->generate({ .volume = volume, .layers = *this, .desiredLevel = level, .seed = _globalSeed });
			if (result._state == GenerationState::Complete)
			{
				result._instance = static_cast<T*>(_typedLayers.at(type).get());
			}
			return result;
		}
		template<typename T>
		FutureLayer<T> generate(entt::id_type id, spatial::AABB<int> volume, int level = 0)
		{
			FutureLayer<T> result;
			result._instance = nullptr;
			result._state = _namedLayers.at(id)->generate({ .volume = volume, .layers = *this, .desiredLevel = level, .seed = _globalSeed });
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
		GenerationChunk(sf::Vector3i index, spatial::AABB<int> volume, LayerType& layer, GenerationLayerManager& layers, unsigned int globalSeed)
			: _index(index)
			, _volume(volume)
			, _layer(layer)
			, _layerManager(layers)
		{
			_localSeed = globalSeed + std::hash<sf::Vector3i>{}(_volume.min);
			_globalSeed = globalSeed;
		}

		virtual GenerationState doGenerate(int level) override final
		{
			if (_currentLevel > numLevels()) return GenerationState::Complete;

			level = level == 0 ? numLevels() : level; // default level "0" generates all layers
			level = std::min(level, numLevels());

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
		virtual bool isGenerated() const override final { return _currentLevel > numLevels(); }

	protected:
		virtual GenerationState generate(int level) { return GenerationState::Complete; }
		virtual int numLevels() const { return 1; }
		template<typename T>
		FutureLayer<T> generateDependency(entt::id_type id, spatial::AABB<int> volume, int level = 0)
		{
			return _layerManager.generate<T>(id, volume, level);
		}
		template<typename T>
		FutureLayer<T> generateDependency(spatial::AABB<int> volume, int level = 0)
		{
			return _layerManager.generate<T>(volume, level);
		}
		spatial::AABB<int> addPaddingToVolume(sf::Vector3i padding)
		{
			spatial::AABB<int> result = _volume;
			result.min -= padding;
			result.max += padding;
			return result;
		}
		void forEachPointInBounds(std::function<void(sf::Vector3i)> func)
		{
			for (int y = _volume.min.y; y < _volume.max.y; y++)
			{
				for (int x = _volume.min.x; x < _volume.max.x; x++)
				{
					for (int z = _volume.min.z; z < _volume.max.z; z++)
					{
						func(sf::Vector3i{ x, y, z });
					}
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
		sf::Vector3i _index;
		spatial::AABB<int> _volume;
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
		GenerationLayer(sf::Vector3i chunkDimensions)
			: _chunkDimensions(chunkDimensions)
		{}

		GenerationState generateNeighborChunks2d(sf::Vector3i chunkCoordinate, details::GenerationContext&& context)
		{
			auto neighbors = spatial::getSurroundingPoints(chunkCoordinate, spatial::PlaneType::XY);
			bool result = generateChunks(neighbors, context);
			if (!result) return GenerationState::Generating;
			return GenerationState::Complete;
		}
		void forEachLoadedNeighborChunk2d(sf::Vector3i chunkCoordinate, std::function<void(const ChunkType&)> func) const
		{
			auto neighbors = spatial::getSurroundingPoints(chunkCoordinate, spatial::PlaneType::XY);
			for (auto&& neighbor : neighbors)
			{
				if (!_chunks.contains(neighbor)) continue;
				func(_chunks.at(neighbor));
			}
		}

		GenerationState generateNeighborChunks3d(sf::Vector3i chunkCoordinate, details::GenerationContext&& context)
		{
			auto neighbors = spatial::getSurroundingPoints(chunkCoordinate);
			bool result = generateChunks(neighbors, context);
			if (!result) return GenerationState::Generating;
			return GenerationState::Complete;
		}
		void forEachLoadedNeighborChunk3d(sf::Vector3i chunkCoordinate, std::function<void(const ChunkType&)> func) const
		{
			auto neighbors = spatial::getSurroundingPoints(chunkCoordinate);
			for (auto&& neighbor : neighbors)
			{
				if (!_chunks.contains(neighbor)) continue;
				func(_chunks.at(neighbor));
			}
		}

	protected:
		sf::Vector3i getChunkDimensions() const
		{
			return _chunkDimensions;
		}

		sf::Vector3i toChunkPosition(sf::Vector3i tilePosition) const
		{
			sf::Vector3i result;
			result.x = tilePosition.x / _chunkDimensions.x;
			result.y = tilePosition.y / _chunkDimensions.y;
			result.z = tilePosition.z / _chunkDimensions.z;
			return result;
		}
		sf::Vector3i toChunkLocalPosition(sf::Vector3i tilePosition) const
		{
			sf::Vector3i result;
			result.x = tilePosition.x % _chunkDimensions.x;
			result.y = tilePosition.y % _chunkDimensions.y;
			result.z = tilePosition.z % _chunkDimensions.z;
			return result;
		}
		sf::Vector3i toTilePosition(sf::Vector3i chunkPosition) const
		{
			sf::Vector3i result;
			result.x = chunkPosition.x * _chunkDimensions.x;
			result.y = chunkPosition.y * _chunkDimensions.y;
			result.z = chunkPosition.z * _chunkDimensions.z;
			return result;
		}
		std::vector<sf::Vector3i> getChunkPointsInsideVolume(drft::spatial::AABB<int> volume) const
		{
			std::vector<sf::Vector3i> result;

			sf::Vector3i min = toChunkPosition(volume.min);
			sf::Vector3i max = toChunkPosition(volume.max - sf::Vector3i{1,1,1});

			for (int x = min.x; x <= max.x; x++)
			{
				for (int y = min.y; y <= max.y; y++)
				{
					for (int z = min.z; z <= max.z; z++)
					{
						result.emplace_back(x, y, z);
					}
				}
			}
			return result;
		}
		std::vector<sf::Vector3i> getChunkPointsInsideArea(sf::IntRect area, sf::Vector3i origin) const
		{
			std::vector<sf::Vector3i> result;
			sf::Vector3i chunkOrigin = toChunkPosition(origin);
			const int z = chunkOrigin.z;

			sf::Vector3i top_left_point = toChunkPosition(spatial::vec3FromPlanar(area.position));
			sf::Vector3i bottom_right_point = toChunkPosition(spatial::vec3FromPlanar(area.position + area.size));
			for (int y = top_left_point.y; y <= bottom_right_point.y; ++y)
			{
				for (int x = top_left_point.x; x <= bottom_right_point.x; ++x)
				{
					result.push_back({ x, y, z});
				}
			}
			return result;
		}

		const ChunkType* tryGetChunk(sf::Vector3i tilePosition) const
		{
			const sf::Vector3i chunkPosition = toChunkPosition(tilePosition);
			if (_chunks.contains(chunkPosition))
			{
				return &_chunks.at(chunkPosition);
			}
			return nullptr;
		}
		void forEachLoadedChunk(std::function<void(ChunkType&)> func)
		{
			for (ChunkType&& chunk : _chunks)
			{
				func(chunk);
			}
		}
		void forEachLoadedChunkInVolume(spatial::AABB<int> volume, std::function<void(ChunkType&)> func)
		{
			auto chunkPoints = getChunkPointsInsideVolume(volume);
			for (auto&& chunkPoint : chunkPoints)
			{
				if (!_chunks.contains(chunkPoint)) continue;
				ChunkType& chunk = _chunks.at(chunkPoint);
				func(chunk);
			}
		}
		void forEachLoadedChunkInArea(sf::IntRect area, sf::Vector3i origin, std::function<void(ChunkType&)> func)
		{
			auto chunkPoints = getChunkPointsInsideArea(area, origin);
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
			const auto chunks = getChunkPointsInsideVolume(context.volume);
			const bool chunksReady = generateChunks(chunks, context);
			if (!chunksReady) return GenerationState::Generating;

			return GenerationState::Complete;
		}
		bool generateChunks(const std::vector<sf::Vector3i>& chunks, details::GenerationContext& context)
		{
			bool result = true;
			for (auto&& point : chunks)
			{
				if (!_chunks.contains(point))
				{
					ChunkType chunk = ChunkType{
						point,
						spatial::AABB<int>{toTilePosition(point), _chunkDimensions},
						*static_cast<LayerType*>(this),
						context.layers,
						context.seed
					};
					_chunks.emplace(point, std::move(chunk));
				}

				GenerationState state = _chunks.at(point).doGenerate(context.desiredLevel);
				if (state == GenerationState::Generating)
				{
					result = false;
				}
			}
			return result;
		}

	private:
		std::unordered_map<sf::Vector3i, ChunkType> _chunks;
		sf::Vector3i _chunkDimensions;
	};

	class OnDemandLayer : public details::AbstractOnDemandLayer, public IGetValueAt
	{
	public:
		virtual double getValueAt(sf::Vector3i tilePosition) = 0;
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
}
