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
#include "Systems/Core/EffectSystem.h"
#include "Systems/Gameplay/BodyPartSystem.h"
#include "Systems/Gameplay/HealthSystem.h"
#include "Systems/Gameplay/StaminaSystem.h"
#include "Systems/Gameplay/HotbarSystem.h"
#include "Systems/Gameplay/Actions/SprintingSystem.h"
#include "Systems/Gameplay/DayNightCycleSystem.h"
#include "Systems/Gameplay/DeathSystem.h"
#include "Systems/Gameplay/ProjectileSystem.h"
#include "Systems/Gameplay/Actions/DropItemSystem.h"
#include "Systems/Gameplay/Actions/EquipItemSystem.h"
#include "Systems/Gameplay/Actions/CraftItemSystem.h"
#include "Systems/Gameplay/Actions/MovementSystem.h"
#include "Systems/Gameplay/Actions/WaitingSystem.h"
#include "Systems/Gameplay/Actions/PickUpSystem.h"
#include "Systems/Gameplay/Actions/InteractionSystem.h"
#include "Systems/Gameplay/Actions/LaunchAttackSystem.h"
#include "Systems/Gameplay/Actions/SelectDirectionSystem.h"
#include "Systems/Gameplay/Actions/TargetSelectSystem.h"
#include "Systems/Gameplay/LightSourceSystem.h"
#include "Systems/Gameplay/LiquidSystem.h"
#include "Systems/Gameplay/QuestingSystem.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Systems/Gameplay/LevelingSystem.h"
#include "Systems/Gameplay/TickingLifetimeSystem.h"
#include "Systems/Gameplay/Actions/OpenEquipmentSystem.h"
#include "Systems/Gameplay/Actions/OpenCraftingSystem.h"
#include "Systems/Gameplay/Actions/OpenWorldMapSystem.h"
#include "Systems/Gameplay/DetermineCraftableItemsSystem.h"
#include "Systems/Gameplay/Actions/UseItemSystem.h"
#pragma endregion
#pragma region Component Includes
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Components/Meta.h"
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

drft::GameState::GameState(StateStack& stack, StateContext& context) 
	: State(stack, context)
{
	init();
}

void drft::GameState::init()
{
	std::cout << "Initializing GameState..." << std::endl;

	loadOrCreateGameSeed();

	_systems = std::make_unique<system::SystemScheduler>(getContext().registry);
	_world = std::make_unique<spatial::WorldGrid>();
	_worldMap = std::make_unique<WorldMap>();
	_factory = std::make_unique<EntityFactory>();
	_dispatcher = std::make_unique<entt::dispatcher>();

	_worldMap->init({ 160, 90 }, rng::RandomNumberGenerator::getSeed());

	connectEventHandlers();
	setupRegistryContext();
	importSystems();
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

void drft::GameState::loadOrCreateGameSeed()
{
	if (std::filesystem::exists(GAME_STATE_SAVE_FILENAME.data()))
	{
		unsigned int seed = 0;
		std::ifstream ifs(GAME_STATE_SAVE_FILENAME.data());
		{
			cereal::JSONInputArchive iarchive(ifs);
			iarchive(cereal::make_nvp("GameSeed", seed));
		}
		rng::RandomNumberGenerator::setSeed(seed);
	}
	else
	{
		rng::RandomNumberGenerator::setSeed(rng::generateSeed());
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
		_player.patch<component::Position>([startingPosition](component::Position& pos)
			{
				pos.position = startingPosition;
			});
		return true;
	}
	return true;
}

void drft::GameState::loadEntityPrototypes()
{
	_factory->loadPrototypes("bases.json");
	_factory->loadPrototypes("materials.json");
	_factory->loadPrototypes("prototypes.json");
	_factory->loadPrototypes("wearables.json");
	_factory->loadPrototypes("player.json");
}

void drft::GameState::setupRegistryContext()
{
	using namespace entt::literals;

	getContext().registry.ctx().emplace<system::InputBuffer&>(_inputBuffer);
	getContext().registry.ctx().emplace<spatial::WorldGrid&>(*_world);
	getContext().registry.ctx().emplace<WorldMap&>(*_worldMap);
	getContext().registry.ctx().emplace<sf::RenderWindow&>(getContext().window);
	getContext().registry.ctx().emplace_as<sf::Texture&>("sprites"_hs, getContext().textures.get("Sprites"));
	getContext().registry.ctx().emplace_as<sf::Texture&>("icons"_hs, getContext().textures.get("Icons"));
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
			oarchive(cereal::make_nvp("GameSeed", rng::RandomNumberGenerator::getSeed()));
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

	_systems->add<RealityBubble>(					Phase::OnPreUpdate);
	_systems->add<TurnManager>(						Phase::OnPreUpdate + 5);

	_systems->add<PlayerInput>(						Phase::OnProcessInput);
	_systems->add<ArtificialInput>(					Phase::OnProcessInput);

	_systems->add<ProjectileSystem>(				Phase::OnUpdate);
	_systems->add<MovementSystem>(					Phase::OnUpdate);
	_systems->add<InteractionSystem>(				Phase::OnUpdate);
	_systems->add<SelectDirectionSystem>(			Phase::OnUpdate);
	_systems->add<WaitingSystem>(					Phase::OnUpdate);
	_systems->add<PickUpSystem>(					Phase::OnUpdate);
	_systems->add<DropItemSystem>(					Phase::OnUpdate);
	_systems->add<EquipItemSystem>(					Phase::OnUpdate);
	_systems->add<CraftItemSystem>(					Phase::OnUpdate);
	_systems->add<UseItemSystem>(					Phase::OnUpdate);
	_systems->add<OpenEquipmentSystem>(				Phase::OnUpdate);
	_systems->add<OpenWorldMapSystem>(				Phase::OnUpdate);
	_systems->add<OpenCraftingSystem>(				Phase::OnUpdate);
	_systems->add<BodyPartSystem>(					Phase::OnUpdate);
	_systems->add<HotbarSystem>(					Phase::OnUpdate);
	_systems->add<LaunchAttackSystem>(				Phase::OnUpdate + 10);
	_systems->add<HealthSystem>(					Phase::OnUpdate + 10);
	_systems->add<StaminaSystem>(					Phase::OnUpdate + 10);
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
	_systems->add<EffectSystem>(					Phase::OnFixedUpdate);
	_systems->add<AnimationSystem>(					Phase::OnFixedUpdate);
	_systems->add<PlayerFOVSystem>(                 Phase::OnFixedUpdate + 5);
	_systems->add<QuestingSystem>(					Phase::OnFixedUpdate + 10);

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
