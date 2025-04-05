#pragma once
#include <functional>
#include <memory>
#include <ProcGen/GenerationState.h>
#include <Spatial/AABB.h>
#include <Spatial/Helpers.h>
#include <unordered_map>
#include <vector>

#include <Utility/StandardLogger.h>
#include <Utility/stdHashing.h>
#include <Utility/DynamicPointerCast.h>
#include <JSON/ICreateFromJson.h>

namespace drft
{
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
			virtual ~AbstractLayer() {};
			virtual GenerationState generate(GenerationContext&& context) = 0;
		};

		class AbstractChunk
		{
		public:
			virtual ~AbstractChunk() {};
			virtual GenerationState doGenerate(int level) = 0;
			virtual bool isGenerated() const = 0;
		};
	}

	class IGetValueAtLayer : public ICreateFromJson
	{
	public:
		virtual double getValueAt(sf::Vector3i position) = 0;
		virtual void createFromJson(const rapidjson::Value& json) = 0;
	};

	class OnDemandLayer : public details::AbstractLayer, public IGetValueAtLayer
	{
	public:
		virtual void createFromJson(const rapidjson::Value& json) = 0;
		virtual double getValueAt(sf::Vector3i tilePosition) = 0;
		virtual GenerationState generate(details::GenerationContext&& context) override
		{
			return GenerationState::Complete;
		}

	protected:
		unsigned int getGlobalSeed() const
		{
			return _globalSeed;
		}

	private:
		friend class GenerationLayerManager;
		unsigned int _globalSeed;
	};

	template<typename T>
	concept DerivedLayer = std::is_base_of<details::AbstractLayer, T>::value;

	template<typename T>
	concept GetValueAtLayer = std::is_base_of<drft::IGetValueAtLayer, T>::value;

	template<typename T>
	concept DerivedOrGetValueLayer = DerivedLayer<T> || GetValueAtLayer<T>;

	template<DerivedOrGetValueLayer T>
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
					LOG_ERROR("Layer id cannot be converted to {}", typeid(T).name());
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
		GenerationLayerManager() = default;
		~GenerationLayerManager() = default;

		GenerationLayerManager(const GenerationLayerManager&) = delete;
		GenerationLayerManager& operator= (const GenerationLayerManager&) = delete;

		GenerationLayerManager(GenerationLayerManager&&) = default;
		GenerationLayerManager& operator= (GenerationLayerManager&&) = default;

		void setSeed(unsigned int seed)
		{
			_globalSeed = seed;
		}

		template<DerivedLayer T>
		FutureLayer<T> generate(spatial::AABB<int> volume, int level = 0)
		{
			entt::id_type type = entt::type_index<T>::value();
			return generate<T>(type, volume, level);
		}
		template<DerivedLayer T>
		FutureLayer<T> generate(entt::id_type id, spatial::AABB<int> volume, int level = 0)
		{
			FutureLayer<T> result;
			result._instance = nullptr;
			if (!_layers.contains(id))
			{
				LOG_ERROR("Could not find layer {} with id {}", typeid(T).name(), id);
				result._state = GenerationState::Failed;
			}
			else
			{
				result._state = _layers.at(id)->generate({ .volume = volume, .layers = *this, .desiredLevel = level, .seed = _globalSeed });
			}

			if (result._state == GenerationState::Complete)
			{
				result._instance = dynamic_cast<T*>(_layers.at(id).get());
			}
			return result;
		}
		template<GetValueAtLayer T>
		FutureLayer<T> generate(entt::id_type id, spatial::AABB<int> volume, int level = 0)
		{
			FutureLayer<T> result;
			result._instance = nullptr;
			if (!_layers.contains(id))
			{
				LOG_ERROR("Could not find layer {} with id {}", typeid(T).name(), id);
				result._state = GenerationState::Failed;
			}
			else
			{
				result._state = GenerationState::Complete;
				result._instance = dynamic_cast<T*>(_layers.at(id).get());
			}
			return result;
		}
		
		template<DerivedLayer T>
		void add(std::unique_ptr<T> layer)
		{
			entt::id_type type = entt::type_index<T>::value();
			_layers.emplace(type, std::move(layer));
		}
		template<DerivedOrGetValueLayer T>
		void add(std::unique_ptr<T> layer, entt::id_type id)
		{
			if (auto casted = dynamic_unique_cast<details::AbstractLayer>(std::move(layer)))
			{
				_layers.emplace(id, std::move(casted));
			}
			else
			{
				LOG_WARNING("Failed to convert layer id {}", id);
			}
		}
		template<>
		void add(std::unique_ptr<OnDemandLayer> layer, entt::id_type id)
		{
			layer->_globalSeed = _globalSeed;
			_layers.emplace(id, std::move(layer));
		}

	private:
		using LayerPtr = std::unique_ptr<details::AbstractLayer>;
		using LayerIdMap = std::unordered_map<entt::id_type, LayerPtr>;
		LayerIdMap _layers;
		unsigned int _globalSeed = 0;
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

		template<DerivedOrGetValueLayer T>
		FutureLayer<T> generateDependency(entt::id_type id, spatial::AABB<int> volume, int level = 0)
		{
			return _layerManager.generate<T>(id, volume, level);
		}

		template<DerivedLayer T>
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
}
