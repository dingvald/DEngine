#include "pch.h"
#include "GameState.h"

#include "Random/RandomNumberGenerator.h"

#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Events/RequestStateChange.h"
#pragma region System Includes
#include "Systems/SystemScheduler.h"
#include "Systems/Core/TileRenderer.h"
#include "Systems/Core/EntityRenderer.h"
#include "Systems/Core/HUD.h"
#include "Systems/Core/RealityBubble.h"
#include "Systems/Core/PlayerInput.h"
#include "Systems/Core/ArtificialInput.h"
#include "Systems/Core/Camera.h"
#include "Systems/Core/ChunkManager.h"
#include "Systems/Core/ItemUniqueIDGenerator.h"
#include "Systems/Core/TurnManager.h"
#include "Systems/Core/WorldGridResolver.h"
#include "Systems/Gameplay/DamageSystem.h"
#include "Systems/Gameplay/DeathSystem.h"
#include "Systems/Gameplay/MovementSystem.h"
#include "Systems/Gameplay/PickUpSystem.h"
#include "Systems/Gameplay/LaunchAttackSystem.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Systems/Gameplay/OpenInventorySystem.h"
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
static constexpr std::string_view GAME_STATE_SAVE_FILENAME = ".\\data\\savegame\\gamestate.json"; // file extension added because it will be fixed

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
	_factory = std::make_unique<EntityFactory>();
	_dispatcher = std::make_unique<entt::dispatcher>();

	connectEventHandlers();
	setupRegistryContext();
	importSystems();
	loadEntityPrototypes();
	loadOrCreatePlayer();

	std::cout << "Starting Gamestate" << std::endl;
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

void drft::GameState::loadOrCreatePlayer()
{
	std::string playerDataPath = std::string(SAVE_DIRECTORY.data()) + PLAYER_FILE_NAME.data() + ".json";

	if (std::filesystem::exists(playerDataPath))
	{
		_player = { getContext().registry, getContext().registry.create()};
		_player = util::loadEntityFromFile(_player, SAVE_DIRECTORY.data(), PLAYER_FILE_NAME.data(), util::SerializeOption::JSON);
	}
	else
	{
		assert(_factory->has("Player"), "No player prototype found - is JSON loaded?");
		_player = _factory->build("Player", getContext().registry);
	}
}

void drft::GameState::loadEntityPrototypes()
{
	_factory->loadPrototypes("materials.json");
	_factory->loadPrototypes("prototypes.json");
	_factory->loadPrototypes("player.json");
}

void drft::GameState::setupRegistryContext()
{
	using namespace entt::literals;

	getContext().registry.ctx().emplace<spatial::WorldGrid&>(*_world);
	getContext().registry.ctx().emplace<sf::Window&>(getContext().window);
	getContext().registry.ctx().emplace_as<sf::Texture&>("sprites"_hs, getContext().textures.get("Sprites"));
	getContext().registry.ctx().emplace<EntityFactory&>(*_factory);
	getContext().registry.ctx().emplace<entt::dispatcher&>(*_dispatcher);
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
			break;
	}

	return false;
}

bool drft::GameState::update(const float dt)
{
	_systems->update(dt);
	return true;
}

void drft::GameState::render(sf::RenderTarget& target)
{	
	_systems->render(target);
}

void drft::GameState::onPop()
{
	// Save game state
	util::saveEntityToFile(_player, SAVE_DIRECTORY.data(), PLAYER_FILE_NAME.data(), util::SerializeOption::JSON);

	std::ofstream ofs{ GAME_STATE_SAVE_FILENAME.data()};
	{
		cereal::JSONOutputArchive oarchive(ofs);
		oarchive(cereal::make_nvp("GameSeed", rng::RandomNumberGenerator::getSeed()));
		_systems->saveAll(oarchive);
	}
	_systems->shutdownAll();
	util::saveRegistryToFile(getContext().registry, SAVE_DIRECTORY.data(), "registry", util::SerializeOption::JSON);
	getContext().registry = entt::registry{}; // There was a bug when calling registry::clear
}

void drft::GameState::importSystems()
{
	std::cout << "Importing Systems..." << std::endl;

	using namespace system;

	// Import all systems into game state
	// Add an offset to adjust execution order of systems
	_systems->add<TurnManager>(				Phase::OnPreUpdate);
	_systems->add<PlayerInput>(				Phase::OnProcessInput);
	_systems->add<ArtificialInput>(			Phase::OnProcessInput);
	_systems->add<MovementSystem>(			Phase::OnUpdate);
	_systems->add<PickUpSystem>(			Phase::OnUpdate);
	_systems->add<OpenInventorySystem>(		Phase::OnUpdate);
	_systems->add<LaunchAttackSystem>(		Phase::OnUpdate + 10);
	_systems->add<DamageSystem>(			Phase::OnUpdate + 10);
	_systems->add<DeathSystem>(				Phase::OnUpdate + 15);
	_systems->add<Camera>(					Phase::OnPostUpdate);
	_systems->add<ChunkManager>(			Phase::OnPostUpdate);
	_systems->add<RealityBubble>(			Phase::OnValidation);
	_systems->add<TileRenderer>(			Phase::OnRender);
	_systems->add<EntityRenderer>(			Phase::OnRender + 5);
	_systems->add<HUD>(						Phase::OnRender + 10);
	_systems->add<WorldGridResolver>(		Phase::Reactive);
	_systems->add<FactionSystem>(			Phase::Reactive);
	_systems->add<ItemUniqueIDGenerator>(	Phase::Reactive);

	_systems->initAll();
}

void drft::GameState::onRequestStatePush(const drft::events::RequestStateStackPush& ev)
{
	this->requestStackPush(ev.stateID);
}
