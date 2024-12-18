#include "pch.h"
#include "GameState.h"

#include <Engine/CommonEngineDirectories.h>

#include "Events/RequestStateChange.h"
#include "ProcGen/WorldGeneration/WorldGenerator.h"

#include <Engine/States/GameStates/CraftingState.h>
#include <Engine/States/GameStates/GameOverState.h>
#include <Engine/States/GameStates/InventoryState.h>
#include <Engine/States/GameStates/SelectDirectionState.h>
#include <Engine/States/GameStates/SelectTargetState.h>
#include <Engine/States/GameStates/SimulationState.h>

#include <JSON/JSONHelpers.h>
#include "Utility/StandardLogger.h"
#include "Factory/EntityFactory.h"


drft::GameState::GameState(StateStack& stack, StateContext& context)
	: State(stack, context)
	, _gameStateStack(context)
{
	std::cout << "Initializing GameState..." << std::endl;

	_worldGenerator = std::make_unique<gen::WorldGenerator>();
	_factory = std::make_unique<EntityFactory>();
	_dispatcher = std::make_unique<entt::dispatcher>();

	registerGameStates();
	connectEventHandlers();
	setupRegistryContext();

	loadOrCreateWorldGenerator();
	loadEntityPrototypes();

	_gameStateStack.pushState(drft::States::Simulation);
}

void drft::GameState::registerGameStates()
{
	_gameStateStack.registerState<SimulationState>(States::Simulation);
	_gameStateStack.registerState<InventoryState>(States::Inventory);
	_gameStateStack.registerState<CraftingState>(States::Crafting);
	_gameStateStack.registerState<SelectDirectionState>(States::SelectDirection);
	_gameStateStack.registerState<SelectTargetState>(States::SelectTarget);
	_gameStateStack.registerState<GameOverState>(States::GameOver);
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
			error_logger << "Error: " << WORLD_GENERATION_FILE_PATH << " could not be parsed." << std::endl;
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

void drft::GameState::loadEntityPrototypes()
{
	_factory->loadPrototypes(ENTITIES_DIRECTORY);
}

void drft::GameState::setupRegistryContext()
{
	using namespace entt::literals;

	getContext().registry.ctx().emplace<system::InputBuffer&>(_inputBuffer);
	getContext().registry.ctx().emplace<gen::WorldGenerator&>(*_worldGenerator);
	getContext().registry.ctx().emplace<sf::RenderWindow&>(getContext().window);
	getContext().registry.ctx().emplace<TextureAtlas&>(getContext().textures);
	getContext().registry.ctx().emplace<const ControlsContext&>(getContext().controls);
	getContext().registry.ctx().emplace_as<sf::Font&>("terminus"_hs, getContext().fonts.get("Terminus"));
	getContext().registry.ctx().emplace<EntityFactory&>(*_factory);
	getContext().registry.ctx().emplace<entt::dispatcher&>(*_dispatcher);
}

bool drft::GameState::handleEvent(const sf::Event& ev)
{	
	if (_gameStateStack.handleEvent(ev)) return true;

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
	_gameStateStack.update(dt);
	return true;
}

bool drft::GameState::fixedUpdate()
{
	_gameStateStack.fixedUpdate();
	return true;
}

void drft::GameState::render(sf::RenderTarget& target)
{	
	_gameStateStack.render(target);
}

void drft::GameState::onPop()
{	
	_gameStateStack.clearStatesNow();
	getContext().registry = entt::registry{}; // There was a bug when calling registry::clear
}

void drft::GameState::onRequestStatePush(const drft::events::RequestStateStackPush& ev)
{
	_gameStateStack.pushState(ev.stateID);
}
