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
#include <JSON/ICreateFromJson.h>
#include <ProcGen/GenerationRegistries.h>
#include <ProcGen/LayeredProcGen/CanvasLayer.h>
#include <ProcGen/GenerationMode.h>

namespace drft
{
	enum class GenerationLevel : unsigned int
	{
		All = 0,
		One,
		Two,
		Three,
		Four,
		Five,
		MAX
		// If you need more than five, perhaps reconsider your design
	};

	class IGetValueAtLayer : public ICreateFromJson
	{
	public:
		virtual void createFromJson(const rapidjson::Value& json) = 0;
		virtual double getValueAt(sf::Vector3i tilePosition) = 0;
	};

	class GenerationLayerManager;

	namespace details
	{
		struct GenerationContext
		{
			spatial::AABB<int> volume;
			GenerationLevel desiredLevel = GenerationLevel::All;
			unsigned int seed;
		};

		class AbstractLayer
		{
		public:
			AbstractLayer(GenerationLayerManager& generationLayerManager, const GenerationRegistries& registries)
				: _manager(generationLayerManager)
				, _registries(registries)
			{
			}

			virtual GenerationState generate(GenerationContext&& context) = 0;
			virtual void cleanup(spatial::AABB<int> volume) {};
			const GenerationRegistries& getRegistries() const { return _registries; }
			GenerationLayerManager& getLayerManager() { return _manager; }

		private:
			std::reference_wrapper<GenerationLayerManager> _manager;
			std::reference_wrapper<const GenerationRegistries> _registries;		
		};

		class AbstractChunk
		{
		public:
			virtual ~AbstractChunk() {};
			virtual GenerationState doGenerate(GenerationLevel level) = 0;
			virtual bool isGenerated() const = 0;
		};

		template <typename T>
		concept ConvertableLayer =
			std::is_base_of_v<drft::details::AbstractLayer, T>
			&& std::is_base_of_v<drft::IGetValueAtLayer, T>;

		class LayerFactory
		{
		public:
			using FactoryMethod = std::function<drft::details::AbstractLayer* (entt::id_type)>;
		public:

			template<ConvertableLayer T>
			void registerLayer(entt::id_type typeId, FactoryMethod factoryMethod)
			{
				_factories.emplace(typeId, factoryMethod);
			}

			drft::details::AbstractLayer* build(entt::id_type typeId, entt::id_type id) const
			{
				if (!_factories.contains(typeId)) return nullptr;
				return _factories.at(typeId)(id);
			}

		private:
			std::unordered_map<entt::id_type, FactoryMethod> _factories;
		};
	}

	class OnDemandLayer : public details::AbstractLayer, public IGetValueAtLayer
	{
	public:
		using AbstractLayer::AbstractLayer;
		virtual void createFromJson(const rapidjson::Value& json) = 0;
		virtual double getValueAt(sf::Vector3i tilePosition) = 0;
		virtual GenerationState generate(details::GenerationContext&& context) override
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

	template<typename T>
	concept DerivedLayer = std::is_base_of<details::AbstractLayer, T>::value;

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
				if (!_instance) return GenerationState::Failed;
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
		GenerationState _state = GenerationState::Generating;
		T* _instance;
	};

	class GenerationLayerManager
	{
	public:
		GenerationLayerManager(const GenerationRegistries& generationRegistries)
			: _generationRegistries(generationRegistries)
		{}
		~GenerationLayerManager() = default;

		GenerationLayerManager(const GenerationLayerManager&) = delete;
		GenerationLayerManager& operator= (const GenerationLayerManager&) = delete;

		GenerationLayerManager(GenerationLayerManager&&) = default;
		GenerationLayerManager& operator= (GenerationLayerManager&&) = default;

		GenerationMode getGenerationMode() const
		{
			return _generationMode;
		}
		void setGenerationMode(GenerationMode mode)
		{
			_generationMode = mode;
		}

		void setSeed(unsigned int seed)
		{
			_globalSeed = seed;
		}
		unsigned int getSeed() const
		{
			return _globalSeed;
		}

		void setEntityPack(const EntityPack& entityPack)
		{
			_entityPack = &entityPack;
		}
		const EntityPack* tryGetEntityPack() const
		{
			return _entityPack;
		}

		template<typename T>
		FutureLayer<T> generate(spatial::AABB<int> volume, GenerationLevel level = GenerationLevel::All)
		{
			entt::id_type type = entt::type_index<T>::value();
			return generate<T>(type, volume, level);
		}
		template<typename T>
		FutureLayer<T> generate(entt::id_type id, spatial::AABB<int> volume, GenerationLevel level = GenerationLevel::All)
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
				result._state = _layers.at(id)->generate({ 
					.volume = volume, 
					.desiredLevel = level, 
					.seed = _globalSeed
				});
			}

			if (result._state == GenerationState::Complete)
			{
				result._instance = dynamic_cast<T*>(_layers.at(id).get());
				if (!result._instance)
				{
					LOG_ERROR("Could not convert layer id {} to {}", id, typeid(T).name());
				}
			}
			return result;
		}

		void cleanup(spatial::AABB<int> volume)
		{
			for (auto&& [id, layer] : _layers)
			{
				layer->cleanup(volume);
			}
		}
		
		template<DerivedLayer T>
		T* add()
		{
			entt::id_type type = entt::type_index<T>::value();
			return add<T>(type);
		}
		template<DerivedLayer T>
		T* add(entt::id_type id)
		{
			_layers.emplace(id, std::make_unique<T>(*this, _generationRegistries));
			return dynamic_cast<T*>(_layers.at(id).get());
		}
		template<typename T>
		T* add(entt::id_type typeId, entt::id_type id)
		{
			auto layerPtr = _layerFactory.build(typeId, id);
			if (!layerPtr) return nullptr;

			return dynamic_cast<T*>(layerPtr);
		}

		CanvasLayer& getCanvas(entt::id_type id)
		{
			if (!_canvasLayers.contains(id))
			{
				_canvasLayers.emplace(id, CanvasLayer{});
			}
			return _canvasLayers.at(id);
		}

		template<details::ConvertableLayer T>
		void registerType(entt::id_type typeId)
		{
			_layerFactory.registerLayer<T>(typeId, [this](entt::id_type id) { return this->add<T>(id); });
		}

	private:
		using GenLayerPtr = std::unique_ptr<details::AbstractLayer>;
		using GenLayerIdMap = std::unordered_map<entt::id_type, GenLayerPtr>;
		using CanvasLayerIdMap = std::unordered_map<entt::id_type, CanvasLayer>;

		GenLayerIdMap _layers;
		CanvasLayerIdMap _canvasLayers;
		details::LayerFactory _layerFactory;
		unsigned int _globalSeed = 0;
		const GenerationRegistries& _generationRegistries;
		const EntityPack* _entityPack = nullptr;
		GenerationMode _generationMode = GenerationMode::OnePerFrame;
	};

    template<typename LayerType, typename ChunkType>
    class GenerationChunk : public details::AbstractChunk
    {
    public:
		GenerationChunk(sf::Vector3i index, spatial::AABB<int> volume, LayerType& layer)
			: _index(index)
			, _volume(volume)
			, _layer(layer)
		{
			_globalSeed = _layer.getLayerManager().getSeed();
			_localSeed = _globalSeed + std::hash<sf::Vector3i>{}(_volume.min);
		}

		GenerationChunk(const GenerationChunk&) = delete;
		GenerationChunk& operator=(const GenerationChunk&) = delete;

        GenerationChunk(GenerationChunk&&) = default;
        GenerationChunk& operator=(GenerationChunk&&) = default;

		virtual GenerationState doGenerate(GenerationLevel desiredLevel) override final
		{
			if (_currentLevel > numLevels()) return GenerationState::Complete;

			desiredLevel = desiredLevel == GenerationLevel::All ? numLevels() : desiredLevel;
			desiredLevel = std::min(desiredLevel, numLevels());

			if (_currentLevel > desiredLevel) return GenerationState::Complete;

			GenerationState state = this->generate(_currentLevel);
			if (state == GenerationState::Complete)
			{
				unsigned int intLevel = static_cast<unsigned int>(_currentLevel);
				intLevel++;
				if (intLevel >= static_cast<unsigned int>(GenerationLevel::MAX))
				{
					intLevel = static_cast<unsigned int>(GenerationLevel::Five);
				}
				_currentLevel = static_cast<GenerationLevel>(intLevel);

				if (_currentLevel <= desiredLevel)
				{
					state = GenerationState::Generating;
				}
			}
			return state;
		};
		virtual bool isGenerated() const override final { return _currentLevel > numLevels(); }

		unsigned int getLocalSeed() const
		{
			return _localSeed;
		}
		unsigned int getGlobalSeed() const
		{
			return _globalSeed;
		}

		int getNumberOfGenerationRequests()
		{
			return _numberOfRequests;
		}
		void incrementGenerationRequests()
		{
			_numberOfRequests++;
		}
		void decrementGenerationRequests()
		{
			_numberOfRequests--;
		}

    protected:
		virtual GenerationState generate(GenerationLevel desiredLevel) { return GenerationState::Complete; }
		virtual GenerationLevel numLevels() const { return GenerationLevel::One; }

		template<typename T>
		FutureLayer<T> generateDependency(entt::id_type id, spatial::AABB<int> volume, GenerationLevel level = GenerationLevel::All)
		{
			return _layer.getLayerManager().generate<T>(id, volume, level);
		}
		template<typename T>
		FutureLayer<T> generateDependency(spatial::AABB<int> volume, GenerationLevel level = GenerationLevel::All)
		{
			return _layer.getLayerManager().generate<T>(volume, level);
		}

		GenerationState generateNeighborChunks2d(GenerationLevel desiredLevel)
		{
			return _layer.accessor.generateNeighborChunks2d(_layer, _index, desiredLevel);
		}
		void forEachLoadedNeighborChunk2d(std::function<void(const ChunkType&)> func) const
		{
			_layer.accessor.forEachLoadedNeighborChunk2d(_layer, _index, func);
		}
		GenerationState generateNeighborChunks3d(GenerationLevel desiredLevel)
		{
			return _layer.accessor.generateNeighborChunks3d(_layer, _index, desiredLevel);
		}
		void forEachLoadedNeighborChunk3d(std::function<void(const ChunkType&)> func) const
		{
			_layer.accessor.forEachLoadedNeighborChunk3d(_layer, _index, func);
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

    protected:
		sf::Vector3i _index;
		spatial::AABB<int> _volume;
		LayerType& _layer;

    private:
		unsigned int _localSeed;
		unsigned int _globalSeed;
		GenerationLevel _currentLevel = GenerationLevel::One;
		int _numberOfRequests = 0;
    };

	template<typename LayerType, typename ChunkType>
	class GenerationLayer : public details::AbstractLayer
	{
	public:
		using AbstractLayer::AbstractLayer;

		class Accessor
		{
		private:
			template<typename LayerType, typename ChunkType>
			friend class GenerationChunk;
			GenerationState generateNeighborChunks2d(GenerationLayer<LayerType, ChunkType>& layer, sf::Vector3i chunkCoordinate, GenerationLevel desiredLevel)
			{
				auto neighbors = spatial::getSurroundingPoints(chunkCoordinate, spatial::PlaneType::XY);
				return layer.generateChunks(neighbors, desiredLevel);
			}
			void forEachLoadedNeighborChunk2d(GenerationLayer<LayerType, ChunkType>& layer, sf::Vector3i chunkCoordinate, std::function<void(const ChunkType&)> func) const
			{
				auto neighbors = spatial::getSurroundingPoints(chunkCoordinate, spatial::PlaneType::XY);
				for (auto&& neighbor : neighbors)
				{
					if (!layer._chunks.contains(neighbor)) continue;
					func(layer._chunks.at(neighbor));
				}
			}
			GenerationState generateNeighborChunks3d(GenerationLayer<LayerType, ChunkType>& layer, sf::Vector3i chunkCoordinate, GenerationLevel desiredLevel)
			{
				auto neighbors = spatial::getSurroundingPoints(chunkCoordinate);
				return layer.generateChunks(neighbors, desiredLevel);
			}
			void forEachLoadedNeighborChunk3d(GenerationLayer<LayerType, ChunkType>& layer, sf::Vector3i chunkCoordinate, std::function<void(const ChunkType&)> func) const
			{
				auto neighbors = spatial::getSurroundingPoints(chunkCoordinate);
				for (auto&& neighbor : neighbors)
				{
					if (!layer._chunks.contains(neighbor)) continue;
					func(layer._chunks.at(neighbor));
				}
			}
		};
		
		Accessor accessor;
	protected:
		virtual sf::Vector3i getChunkDimensions() const = 0;

		sf::Vector3i toChunkPosition(sf::Vector3i tilePosition) const
		{
			sf::Vector3i result;
			const sf::Vector3i chunkDimensions = getChunkDimensions();
			result.x = (tilePosition.x >= 0) ? (tilePosition.x / chunkDimensions.x)
				: ((tilePosition.x + 1) / chunkDimensions.x - 1);
			result.y = (tilePosition.y >= 0) ? (tilePosition.y / chunkDimensions.y)
				: ((tilePosition.y + 1) / chunkDimensions.y - 1);
			result.z = (tilePosition.z >= 0) ? (tilePosition.z / chunkDimensions.z)
				: ((tilePosition.z + 1) / chunkDimensions.z - 1);
			return result;
		}
		sf::Vector3i toTilePosition(sf::Vector3i chunkPosition) const
		{
			return chunkPosition.componentWiseMul(getChunkDimensions());
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
		std::vector<sf::Vector3i> getChunkPointsInsideArea(sf::IntRect area, int z) const
		{
			std::vector<sf::Vector3i> result;

			const sf::Vector3i zChunk = toChunkPosition({ 0,0,z });

			sf::Vector3i top_left_point = toChunkPosition(spatial::vec3FromPlanar(area.position));
			sf::Vector3i bottom_right_point = toChunkPosition(spatial::vec3FromPlanar(area.position + area.size - sf::Vector2i{1, 1}));
			for (int y = top_left_point.y; y <= bottom_right_point.y; ++y)
			{
				for (int x = top_left_point.x; x <= bottom_right_point.x; ++x)
				{
					result.push_back({ x, y, zChunk.z });
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
		void forEachLoadedChunkInArea(sf::IntRect area, int z, std::function<void(ChunkType&)> func)
		{
			auto chunkPoints = getChunkPointsInsideArea(area, z);
			for (auto&& chunkPoint : chunkPoints)
			{
				if (!_chunks.contains(chunkPoint)) continue;
				ChunkType& chunk = _chunks.at(chunkPoint);
				func(chunk);
			}
		}

		void removeChunksInVolume(spatial::AABB<int> volume)
		{
			auto chunkPoints = getChunkPointsInsideVolume(volume);
			_chunks.erase(chunkPoints.begin(), chunkPoints.end());
		}
		void removeChunksInArea(sf::IntRect area, int z)
		{
			auto chunks = getChunkPointsInsideArea(area, z);
			for (auto&& chunk : chunks)
			{
				_chunks.erase(chunk);
			}
		}

	private:
		friend class Accessor;
		GenerationState generate(details::GenerationContext&& context) override final
		{
			const auto chunks = getChunkPointsInsideVolume(context.volume);
			return generateChunks(chunks, context.desiredLevel);
		}
		GenerationState generateChunks(const std::vector<sf::Vector3i>& chunks, GenerationLevel desiredLevel)
		{
			GenerationMode mode = getLayerManager().getGenerationMode();
			switch (mode)
			{
			case GenerationMode::OnePerFrame:
				return stepGenerate(chunks, desiredLevel);
			case GenerationMode::Batch:
				return batchGenerate(chunks, desiredLevel);
			default:
				throw std::exception("Unknown enum");
				return GenerationState::Failed;
			}
		}
		void cleanup(spatial::AABB<int> volume) override
		{
			auto chunks = getChunkPointsInsideVolume(volume);
			for (auto&& pos : chunks)
			{
				_chunks.erase(pos);
			}
		}

		ChunkType& getOrCreateChunk(sf::Vector3i point)
		{
			if (!_chunks.contains(point))
			{
				_chunks.emplace(point, ChunkType{
						point, spatial::AABB<int>{ toTilePosition(point), getChunkDimensions() },
						*static_cast<LayerType*>(this) });
			}
			return _chunks.at(point);
		}
		GenerationState batchGenerate(const std::vector<sf::Vector3i>& chunks, GenerationLevel desiredLevel)
		{
			GenerationState result = GenerationState::Complete;
			for (auto&& point : chunks)
			{
				ChunkType& chunk = getOrCreateChunk(point);

				GenerationState state = chunk.doGenerate(desiredLevel);
				if (state != GenerationState::Complete)
				{
					result = state;
				}
			}
			return result;
		}
		GenerationState stepGenerate(const std::vector<sf::Vector3i>& chunks, GenerationLevel desiredLevel)
		{
			for (auto&& point : chunks)
			{
				ChunkType& chunk = getOrCreateChunk(point);

				GenerationState state = chunk.doGenerate(desiredLevel);
				if (state != GenerationState::Complete) return state;
			}
			return GenerationState::Complete;
		}

	private:
		std::unordered_map<sf::Vector3i, ChunkType> _chunks;
	};
}
