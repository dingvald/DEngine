#include "pch.h"
#include "GameState.h"

#include "Random/RandomNumberGenerator.h"

#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Events/RequestStateChange.h"
#include "WorldMap/WorldMap.h"

#pragma region System Includes
#include "Systems/SystemScheduler.h"
#include "Systems/Rendering/CullingSystem.h"
#include "Systems/Rendering/AnimationSystem.h"
#include "Systems/Rendering/PlayerFOVSystem.h"
#include "Systems/Rendering/EntityRenderer.h"
#include "Systems/Rendering/EffectRenderer.h"
#include "Systems/Rendering/LightingSystem.h"
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
#include "Utility/SaveEntity.h"
#include "Utility/LoadEntity.h"
#include "Utility/SaveRegistry.h"
#include "Utility/LoadRegistry.h"
#include "Factory/EntityFactory.h"


// TODO: Move save directory to state context
static constexpr std::string_view SAVE_DIRECTORY = ".\\data\\savegame\\";
static constexpr std::string_view PLAYER_FILE_NAME = "playerSaveData";
static constexpr std::string_view GAME_STATE_SAVE_FILENAME = ".\\data\\savegame\\gamestate.json"; // file extension added because it will always be json

static const std::filesystem::path STATIC_DATA_PATH = ".\\data\\static\\";
static const std::filesystem::path ENTITIES_FOLDER_PATH = STATIC_DATA_PATH.string() + "entities";

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
	_worldMap = std::make_unique<WorldMap>();
	_factory = std::make_unique<EntityFactory>();
	_dispatcher = std::make_unique<entt::dispatcher>();
	
	connectEventHandlers();
	setupRegistryContext();
	importSystems();
	loadOrCreateWorldMap();
	loadEntityPrototypes();
	loadRegistry();

	bool isNewGame = loadOrCreatePlayer();

	std::cout << "Starting Gamestate" << std::endl;
	_systems->startAll(isNewGame);
}

void drft::GameState::connectEventHandlers()
{
	_dispatcher->sink<events::RequestStateStackPush>().connect<&GameState::onRequestStatePush>(this);
}

void drft::GameState::loadOrCreateWorldMap()
{
	if (std::filesystem::exists(GAME_STATE_SAVE_FILENAME.data()))
	{
		std::ifstream ifs(GAME_STATE_SAVE_FILENAME.data());
		{
			cereal::JSONInputArchive iarchive(ifs);
			_worldMap->load(iarchive);
		}
	}
	else
	{
		_worldMap->create();
	}
}

bool drft::GameState::loadOrCreatePlayer()
{
	std::string playerDataPath = std::string(SAVE_DIRECTORY.data()) + PLAYER_FILE_NAME.data() + ".json";

	if (std::filesystem::exists(playerDataPath))
	{
		_player = { getContext().registry, getContext().registry.create()};
		_player = util::loadEntityFromFile(_player, SAVE_DIRECTORY.data(), PLAYER_FILE_NAME.data(), util::SerializeOption::JSON);
		return false;
	}
	else
	{
		assert(_factory->has("Player"), "No player prototype found - is JSON loaded?");
		_player = _factory->build("Player", getContext().registry);
		auto startingPosition = getContext().registry.ctx().get<WorldMap&>().getStartingPosition("Forest");
		_player.patch<PositionComponent>([startingPosition](PositionComponent& pos)
			{
				pos.position = startingPosition;
			});
		return true;
	}
	return true;
}

void drft::GameState::loadEntityPrototypes()
{
	_factory->loadPrototypes(ENTITIES_FOLDER_PATH);
}

void drft::GameState::setupRegistryContext()
{
	using namespace entt::literals;

	getContext().registry.ctx().emplace<system::InputBuffer&>(_inputBuffer);
	getContext().registry.ctx().emplace<spatial::WorldGrid&>(*_world);
	getContext().registry.ctx().emplace<WorldMap&>(*_worldMap);
	getContext().registry.ctx().emplace<sf::RenderWindow&>(getContext().window);
	getContext().registry.ctx().emplace<TextureAtlas&>(getContext().textures);
	getContext().registry.ctx().emplace_as<sf::Font&>("terminus"_hs, getContext().fonts.get("Terminus"));
	getContext().registry.ctx().emplace<EntityFactory&>(*_factory);
	getContext().registry.ctx().emplace<entt::dispatcher&>(*_dispatcher);
}

void drft::GameState::loadRegistry()
{
	if (std::filesystem::exists(GAME_STATE_SAVE_FILENAME.data()))
	{
		util::loadRegistryFromFile(getContext().registry, SAVE_DIRECTORY.data(), "registry", util::SerializeOption::JSON);
	}
}

bool drft::GameState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
		case sf::Event::KeyPressed:
		{
			if (ev.key.code == sf::Keyboard::Escape)
			{
				requestStackPush(States::Pause);
				return false;
			}
			_inputBuffer.pushKey(ev.key.code);
		}
		break;
	}

	return false;
}

bool drft::GameState::update(const float dt)
{
	_systems->update(dt);
	_systems->updateEnd();
	return true;
}

bool drft::GameState::fixedUpdate()
{
	_systems->fixedUpdate();
	_worldMap->fixedUpdate(getContext().registry);
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
		util::saveEntityToFile(_player, SAVE_DIRECTORY.data(), PLAYER_FILE_NAME.data(), util::SerializeOption::JSON);

		std::ofstream ofs{ GAME_STATE_SAVE_FILENAME.data() };
		{
			cereal::JSONOutputArchive oarchive(ofs);

			_worldMap->save(oarchive);
			_systems->saveAll(oarchive);
		}
	}
	if (isPlayerAlive)
	{
		util::saveRegistryToFile(getContext().registry, SAVE_DIRECTORY.data(), "registry", util::SerializeOption::JSON);
	}
	
	getContext().registry = entt::registry{}; // There was a bug when calling registry::clear
}

void drft::GameState::importSystems()
{
	std::cout << "Importing Systems..." << std::endl;

	using namespace system;

	_systems->add<RealityBubble>(					Phase::OnPreUpdate); // This should go first - determines which actors are "active"
	_systems->add<TurnManager>(						Phase::OnPreUpdate + 5);

	_systems->add<PlayerInput>(						Phase::OnProcessInput);
	_systems->add<ArtificialInput>(					Phase::OnProcessInput);

	_systems->add<ProjectileSystem>(				Phase::OnUpdate);
	
	_systems->add<InteractionSystem>(				Phase::OnUpdate);
	_systems->add<WaitingSystem>(					Phase::OnUpdate);
	_systems->add<DropItemSystem>(					Phase::OnUpdate);
	_systems->add<EquipItemSystem>(					Phase::OnUpdate);
	_systems->add<CraftItemSystem>(					Phase::OnUpdate);
	_systems->add<BodyPartSystem>(					Phase::OnUpdate);
	_systems->add<HotbarSystem>(					Phase::OnUpdate);
	_systems->add<HealthSystem>(					Phase::OnUpdate + 10);
	_systems->add<DeathSystem>(						Phase::OnUpdate + 15);
	_systems->add<LevelingSystem>(					Phase::OnUpdate + 20);
	_systems->add<Camera>(							Phase::OnPostUpdate);
	_systems->add<ChunkManager>(					Phase::OnPostUpdate);
	
	_systems->add<DayNightCycleSystem>(				Phase::OnFixedUpdate);
	_systems->add<SprintingSystem>(					Phase::OnFixedUpdate);
	_systems->add<CullingSystem>(					Phase::OnFixedUpdate);
	_systems->add<LiquidSystem>(					Phase::OnFixedUpdate);
	_systems->add<LightSourceSystem>(				Phase::OnFixedUpdate);
	_systems->add<LightingSystem>(					Phase::OnFixedUpdate);
	_systems->add<VisualEffectSystem>(				Phase::OnFixedUpdate);
	_systems->add<AnimationSystem>(					Phase::OnFixedUpdate);
	_systems->add<PlayerFOVSystem>(                 Phase::OnFixedUpdate + 5);

	_systems->add<EntityRenderer>(					Phase::OnRender);
	_systems->add<EffectRenderer>(					Phase::OnRender);
	_systems->add<HUD>(								Phase::OnRender + 5);
	_systems->add<FloatingTextSystem>(				Phase::OnRender + 5);

	_systems->add<WorldGridResolver>(				Phase::Reactive);
	_systems->add<FactionSystem>(					Phase::Reactive);
	_systems->add<ItemUniqueIDGenerator>(			Phase::Reactive);
	_systems->add<DetermineCraftableItemsSystem>(	Phase::Reactive);
	_systems->add<TickingLifetimeSystem>(			Phase::Reactive);
	_systems->add<TargetSelectSystem>(				Phase::Reactive);
	_systems->add<SelectDirectionSystem>(			Phase::Reactive);
	_systems->add<OpenableSystem>(					Phase::Reactive);
	_systems->add<ConsumableSystem>(				Phase::Reactive);
	_systems->add<HealingSystem>(					Phase::Reactive);
	_systems->add<OpenCraftingSystem>(				Phase::Reactive);
	_systems->add<OpenEquipmentSystem>(				Phase::Reactive);
	_systems->add<OpenWorldMapSystem>(				Phase::Reactive);
	_systems->add<PickUpSystem>(					Phase::Reactive);
	_systems->add<MoveActionSystem>(				Phase::Reactive);
	_systems->add<MeleeAttackActionSystem>(			Phase::Reactive);
	_systems->add<CollisionSystem>(					Phase::Reactive);
	_systems->add<StaminaSystem>(					Phase::Reactive);


	if (std::filesystem::exists(GAME_STATE_SAVE_FILENAME.data()))
	{
		std::ifstream ifs(GAME_STATE_SAVE_FILENAME.data());
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
