#include "pch.h"
#include "GameState.h"

#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Events/RequestStateChange.h"
#include "ProcGen/WorldGeneration/WorldGenerator.h"
#include "WorldMap/WorldMap.h"

#pragma region System Includes
#include "Systems/SystemScheduler.h"
#include "Systems/Rendering/CullingSystem.h"
#include "Systems/Rendering/AnimationSystem.h"
#include "Systems/Rendering/SyncedAnimationSystem.h"
#include "Systems/Rendering/PlayerFOVSystem.h"
#include "Systems/Rendering/EntityRenderer.h"
#include "Systems/Rendering/LightingSystem.h"
#include "Systems/Rendering/SpriteControllerSystem.h"
#include "Systems/Core/HUD.h"
#include "Systems/Core/RealityBubble.h"
#include "Systems/Core/PlayerInput.h"
#include "Systems/Core/ArtificialInput.h"
#include "Systems/Core/Camera.h"
#include "Systems/Core/ChunkManager.h"
#include "Systems/Core/ItemUniqueIDGenerator.h"
#include "Systems/Core/TurnManager.h"
#include "Systems/Core/WorldGridResolver.h"
#include "Systems/Core/FloatingTextSystem.h"
#include "Systems/Core/VisualEffectSystem.h"
#include "Systems/Gameplay/BodyPartSystem.h"
#include "Systems/Gameplay/CollisionSystem.h"
#include "Systems/Gameplay/HealthSystem.h"
#include "Systems/Gameplay/StaminaSystem.h"
#include "Systems/Gameplay/HotbarSystem.h"
#include "Systems/Gameplay/Actions/SprintingSystem.h"
#include "Systems/Gameplay/DayNightCycleSystem.h"
#include "Systems/Gameplay/DeathSystem.h"
#include "Systems/Gameplay/ProjectileSystem.h"
#include "Systems/Gameplay/HealingSystem.h"
#include "Systems/Gameplay/Actions/DropItemSystem.h"
#include "Systems/Gameplay/Actions/EquipItemSystem.h"
#include "Systems/Gameplay/Actions/CraftItemSystem.h"
#include "Systems/Gameplay/Actions/MoveActionSystem.h"
#include "Systems/Gameplay/Actions/WaitingSystem.h"
#include "Systems/Gameplay/Actions/PickUpSystem.h"
#include "Systems/Gameplay/Actions/InteractionSystem.h"
#include "Systems/Gameplay/Actions/MeleeAttackActionSystem.h"
#include "Systems/Gameplay/Actions/SelectDirectionSystem.h"
#include "Systems/Gameplay/Actions/TargetSelectSystem.h"
#include "Systems/Gameplay/OpenableSystem.h"
#include "Systems/Gameplay/ConsumableSystem.h"
#include "Systems/Gameplay/LightSourceSystem.h"
#include "Systems/Gameplay/LiquidSystem.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Systems/Gameplay/LevelingSystem.h"
#include "Systems/Gameplay/TickingLifetimeSystem.h"
#include "Systems/Gameplay/Actions/OpenEquipmentSystem.h"
#include "Systems/Gameplay/Actions/OpenCraftingSystem.h"
#include "Systems/Gameplay/Actions/OpenWorldMapSystem.h"
#include "Systems/Gameplay/DetermineCraftableItemsSystem.h"
#pragma endregion
#pragma region Component Includes
#include "Components/PositionComponent.h"
#include "Components/Tags.h"

#pragma endregion

#include <JSON/JSONHelpers.h>
#include "Utility/SaveEntity.h"
#include "Utility/LoadEntity.h"
#include "Utility/SaveRegistry.h"
#include "Utility/LoadRegistry.h"
#include "Utility/StandardLogger.h"
#include "Factory/EntityFactory.h"


// TODO: Move save directory to state context
static const std::filesystem::path WORKING_DIRECTORY = ".";
static const std::filesystem::path SAVE_DIRECTORY = WORKING_DIRECTORY / "data" / "savegame";
static const std::filesystem::path STATIC_DATA_DIRECTORY = WORKING_DIRECTORY / "data" / "static";

static const std::filesystem::path PLAYER_SAVE_FILE_PATH = SAVE_DIRECTORY / "playerSaveData.json";
static const std::filesystem::path GAMESTATE_SAVE_FILE_PATH = SAVE_DIRECTORY / "gamestate.json";
static const std::filesystem::path MAIN_REGISTRY_FILE_PATH = SAVE_DIRECTORY / "registry.json";

static const std::filesystem::path ENTITIES_DIRECTORY = STATIC_DATA_DIRECTORY / "entities";
static const std::filesystem::path WORLD_GENERATION_FILE_PATH = STATIC_DATA_DIRECTORY / "world_generation.json";

drft::GameState::GameState(StateStack& stack, StateContext& context) 
	: State(stack, context)
{
	init();
}

void drft::GameState::init()
{
	std::cout << "Initializing GameState..." << std::endl;

	_systems = std::make_unique<system::SystemScheduler>(getContext().registry);
	_world = std::make_unique<spatial::WorldGrid>();
	_worldGenerator = std::make_unique<gen::WorldGenerator>();
	_worldMap = std::make_unique<WorldMap>(*_worldGenerator);
	_factory = std::make_unique<EntityFactory>();
	_dispatcher = std::make_unique<entt::dispatcher>();
	
	connectEventHandlers();
	setupRegistryContext();
	loadOrCreateWorldGenerator();
	importSystems();
	loadEntityPrototypes();
	loadRegistry();

	bool isNewGame = loadOrCreatePlayer();

	std::cout << "Starting Gamestate..." << std::endl;
	_systems->startAll(isNewGame);
	std::cout << "<<< Game Started >>>" << std::endl;
}

void drft::GameState::connectEventHandlers()
{
	_dispatcher->sink<events::RequestStateStackPush>().connect<&GameState::onRequestStatePush>(this);
}

void drft::GameState::loadOrCreateWorldGenerator()
{
	if (std::filesystem::exists(GAMESTATE_SAVE_FILE_PATH))
	{
		std::ifstream ifs(GAMESTATE_SAVE_FILE_PATH);
		{
			cereal::JSONInputArchive iarchive(ifs);
			_worldGenerator->load(iarchive);
		}
	}
	else
	{
		json::JsonRootExtractor jsonRootExtractor{ WORLD_GENERATION_FILE_PATH, "world_generation" };
		if (!jsonRootExtractor.isValid())
		{
			return;
		}
		else
		{
			_worldGenerator->createFromJson(jsonRootExtractor.getRoot());
		}
	}

	_worldGenerator->init();
	_worldGenerator->generate();
}

bool drft::GameState::loadOrCreatePlayer()
{
	if (std::filesystem::exists(PLAYER_SAVE_FILE_PATH))
	{
		_player = { getContext().registry, getContext().registry.create()};
		util::loadEntityFromFile(_player, PLAYER_SAVE_FILE_PATH);
		return false;
	}
	else
	{
		assert(_factory->has("Player"), "No player prototype found - is JSON loaded?");
		_player = _factory->build("Player", getContext().registry);
		_player.patch<PositionComponent>([](PositionComponent& pos)
			{
				pos.position = {1024, 1024};
			});
		return true;
	}
	return true;
}

void drft::GameState::loadEntityPrototypes()
{
	_factory->loadPrototypes(ENTITIES_DIRECTORY);
}

void drft::GameState::setupRegistryContext()
{
	using namespace entt::literals;

	getContext().registry.ctx().emplace<system::InputBuffer&>(_inputBuffer);
	getContext().registry.ctx().emplace<spatial::WorldGrid&>(*_world);
	getContext().registry.ctx().emplace<gen::WorldGenerator&>(*_worldGenerator);
	getContext().registry.ctx().emplace<WorldMap>(*_worldMap);
	getContext().registry.ctx().emplace<sf::RenderWindow&>(getContext().window);
	getContext().registry.ctx().emplace<TextureAtlas&>(getContext().textures);
	getContext().registry.ctx().emplace_as<sf::Font&>("terminus"_hs, getContext().fonts.get("Terminus"));
	getContext().registry.ctx().emplace<EntityFactory&>(*_factory);
	getContext().registry.ctx().emplace<entt::dispatcher&>(*_dispatcher);
}

void drft::GameState::loadRegistry()
{
	if (std::filesystem::exists(GAMESTATE_SAVE_FILE_PATH))
	{
		util::loadRegistryFromFile(getContext().registry, MAIN_REGISTRY_FILE_PATH);
	}
}

bool drft::GameState::handleEvent(const sf::Event& ev)
{	
	switch (ev.type)
	{
		case sf::Event::KeyPressed:
			if (ev.key.code == sf::Keyboard::Escape)
			{
				requestStackPush(States::Pause);
				return false;
			}
			_inputBuffer.press(ev.key.code);
		break;
		case sf::Event::KeyReleased:
			_inputBuffer.release(ev.key.code);
		break;
	}

	return false;
}

bool drft::GameState::update(const float dt)
{
	_inputBuffer.update(dt);
	_systems->update(dt);
	_systems->updateEnd();
	return true;
}

bool drft::GameState::fixedUpdate()
{
	_systems->fixedUpdate();
	return true;
}

void drft::GameState::render(sf::RenderTarget& target)
{	
	_systems->render(target);
	_systems->fixedUpdateEnd();
}

void drft::GameState::onPop()
{
	_systems->shutdownAll();
	const bool isPlayerAlive = getContext().registry.valid(_player.entity());
	// Save game state
	if (isPlayerAlive)
	{
		util::saveEntityToFile(_player, PLAYER_SAVE_FILE_PATH);

		std::ofstream ofs{ GAMESTATE_SAVE_FILE_PATH };
		{
			cereal::JSONOutputArchive oarchive(ofs);

			_worldGenerator->save(oarchive);
			_systems->saveAll(oarchive);
		}
	}
	if (isPlayerAlive)
	{
		util::saveRegistryToFile(getContext().registry, MAIN_REGISTRY_FILE_PATH);
	}
	
	getContext().registry = entt::registry{}; // There was a bug when calling registry::clear
}

void drft::GameState::importSystems()
{
	std::cout << "Importing Systems..." << std::endl;

	using namespace system;

	_systems->add<RealityBubble>();
	_systems->add<TurnManager>();

	_systems->add<PlayerInput>();
	_systems->add<ArtificialInput>();

	_systems->add<ProjectileSystem>();
	_systems->add<InteractionSystem>();
	_systems->add<WaitingSystem>();
	_systems->add<DropItemSystem>();
	_systems->add<EquipItemSystem>();
	_systems->add<CraftItemSystem>();
	_systems->add<BodyPartSystem>();
	_systems->add<HotbarSystem>();
	_systems->add<HealthSystem>();
	_systems->add<DeathSystem>();
	_systems->add<LevelingSystem>();
	_systems->add<Camera>();
	_systems->add<ChunkManager>();
	
	_systems->add<DayNightCycleSystem>();
	_systems->add<SprintingSystem>();
	_systems->add<CullingSystem>();
	_systems->add<LiquidSystem>();
	_systems->add<PlayerFOVSystem>();
	_systems->add<LightSourceSystem>();
	_systems->add<LightingSystem>();
	_systems->add<VisualEffectSystem>();
	_systems->add<SpriteControllerSystem>();
	_systems->add<AnimationSystem>();
	_systems->add<SyncedAnimationSystem>();

	_systems->add<EntityRenderer>();
	_systems->add<HUD>();
	_systems->add<FloatingTextSystem>();

	_systems->add<WorldGridResolver>();
	_systems->add<FactionSystem>();
	_systems->add<ItemUniqueIDGenerator>();
	_systems->add<DetermineCraftableItemsSystem>();
	_systems->add<TickingLifetimeSystem>();
	_systems->add<TargetSelectSystem>();
	_systems->add<SelectDirectionSystem>();
	_systems->add<OpenableSystem>();
	_systems->add<ConsumableSystem>();
	_systems->add<HealingSystem>();
	_systems->add<OpenCraftingSystem>();
	_systems->add<OpenEquipmentSystem>();
	_systems->add<OpenWorldMapSystem>();
	_systems->add<PickUpSystem>();
	_systems->add<MoveActionSystem>();
	_systems->add<MeleeAttackActionSystem>();
	_systems->add<CollisionSystem>();
	_systems->add<StaminaSystem>();


	if (std::filesystem::exists(GAMESTATE_SAVE_FILE_PATH))
	{
		std::ifstream ifs(GAMESTATE_SAVE_FILE_PATH);
		{
			cereal::JSONInputArchive iarchive(ifs);
			_systems->loadAll(iarchive);
		}
	}
	_systems->initAll();
}

void drft::GameState::onRequestStatePush(const drft::events::RequestStateStackPush& ev)
{
	this->requestStackPush(ev.stateID);
}
