#include "pch.h"
#include "GameState.h"

#include "Random/RandomNumberGenerator.h"

#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"

#include "Systems/SystemScheduler.h"
#include "Systems/Core/TileRenderer.h"
#include "Systems/Core/EntityRenderer.h"
#include "Systems/Core/HUD.h"
#include "Systems/Core/RealityBubble.h"
#include "Systems/Core/PlayerInput.h"
#include "Systems/Core/ArtificialInput.h"
#include "Systems/Core/Camera.h"
#include "Systems/Core/ChunkManager.h"
#include "Systems/Core/TurnManager.h"
#include "Systems/Core/WorldGridResolver.h"
#include "Systems/Gameplay/DamageSystem.h"
#include "Systems/Gameplay/DeathSystem.h"
#include "Systems/Gameplay/MovementSystem.h"
#include "Systems/Gameplay/LaunchAttackSystem.h"
#include "Systems/Gameplay/FactionSystem.h"

#include "Components/Components.h"
#include "Components/Tags.h"
#include "Components/Meta.h"

#include "Utility/TestEntities.h"
#include "Utility/SaveEntity.h"
#include "Utility/LoadEntity.h"
#include "Factory/EntityFactory.h"


// TODO: Move save directory to state context
static constexpr std::string_view SAVE_DIRECTORY = ".\\data\\savegame\\";
static constexpr std::string_view PLAYER_FILE_NAME = "playerSaveData";

drft::GameState::GameState(StateStack& stack, Context context) 
	: State(stack, context)
{
	init();
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
	util::saveEntityToFile(_player, SAVE_DIRECTORY.data(), PLAYER_FILE_NAME.data(), util::SerializeOption::JSON);
	_systems->shutdownAll();
}

void drft::GameState::init()
{
	using namespace entt::literals;

	std::cout << "Initializing GameState..." << std::endl;

	rng::RandomNumberGenerator::setSeed(rng::generateSeed());

	_systems = std::make_unique<system::SystemScheduler>(_registry);
	_world = std::make_unique<spatial::WorldGrid>();
	_factory = std::make_unique<EntityFactory>();
	_dispatcher = std::make_unique<entt::dispatcher>();

	_registry.ctx().emplace<spatial::WorldGrid&>(*_world);
	_registry.ctx().emplace<sf::Window&>(*getContext().window);
	_registry.ctx().emplace_as<sf::Texture&>("sprites"_hs, getContext().textures->get("Sprites"));
	_registry.ctx().emplace<EntityFactory&>(*_factory);
	_registry.ctx().emplace<entt::dispatcher&>(*_dispatcher);
	
	importSystems();

	_factory->loadPrototypes("materials.json", _registry);
	_factory->loadPrototypes("prototypes.json", _registry);
	_factory->loadPrototypes("player.json", _registry);

	std::string fullpath = std::string(SAVE_DIRECTORY.data()) + PLAYER_FILE_NAME.data() + ".json";

	if (std::filesystem::exists(fullpath))
	{
		_player = { _registry, _registry.create() };
		_player = util::loadEntityFromFile(_player, SAVE_DIRECTORY.data(), PLAYER_FILE_NAME.data(), util::SerializeOption::JSON);
	}
	else
	{
		_player = _factory->build("Player", _registry);
	}

	std::cout << "Starting Gamestate" << std::endl;
}

void drft::GameState::importSystems()
{
	std::cout << "Importing Systems..." << std::endl;

	using namespace system;

	// Import all systems into game state
	// Add an offset to adjust execution order of systems
	_systems->add(TurnManager(),		Phase::OnPreUpdate);
	_systems->add(PlayerInput(),		Phase::OnProcessInput);
	_systems->add(ArtificialInput(),	Phase::OnProcessInput);
	_systems->add(MovementSystem(),		Phase::OnUpdate);
	_systems->add(LaunchAttackSystem(), Phase::OnUpdate + 10);
	_systems->add(DamageSystem(),		Phase::OnUpdate + 10);
	_systems->add(DeathSystem(),		Phase::OnUpdate + 15);
	_systems->add(Camera(),				Phase::OnPostUpdate);
	_systems->add(ChunkManager(),		Phase::OnPostUpdate);
	_systems->add(RealityBubble(),		Phase::OnValidation);
	_systems->add(TileRenderer(),		Phase::OnRender);
	_systems->add(EntityRenderer(),		Phase::OnRender);
	_systems->add(HUD(),				Phase::OnRender + 10);
	_systems->add(WorldGridResolver(),	Phase::Reactive);
	_systems->add(FactionSystem(),		Phase::Reactive);

	_systems->initAll();
}
