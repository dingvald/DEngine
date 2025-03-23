#include "pch.h"
#include "GameState.h"

#include <Engine/CommonEngineDirectories.h>
#include <Actions/ActionMap.h>

#include "Events/RequestStateChange.h"
#include "ProcGen/WorldGeneration/WorldGenerator.h"
#include <Keybindings/KeybindingsUtils.h>
#include <Keybindings/Keybindings.h>

#include <States/GameStates/CraftingState.h>
#include <States/GameStates/GameOverState.h>
#include <States/GameStates/InventoryState.h>
#include <States/GameStates/SelectDirectionState.h>
#include <States/GameStates/SelectTargetState.h>
#include <States/GameStates/SimulationState.h>

#include <Systems/Helpers/GetCurrentCamera.h>

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
	setupActionMap();

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
		json::JsonFileWrapper json{ WORLD_GENERATION_FILE_PATH, "world_generation" };
		if (!json.load())
		{
			error_logger << "Error: " << WORLD_GENERATION_FILE_PATH << " could not be loaded." << std::endl;
			return;
		}
		else
		{
			_worldGenerator->createFromJson(json.getRoot());
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
	getContext().registry.ctx().emplace<ActionMap>(getContext().actions);
	getContext().registry.ctx().emplace_as<sf::Font&>("terminus"_hs, getContext().fonts.get("Terminus"));
	getContext().registry.ctx().emplace<EntityFactory&>(*_factory);
	getContext().registry.ctx().emplace<entt::dispatcher&>(*_dispatcher);
	getContext().registry.ctx().emplace<tgui::Gui&>(getContext().gui);
}

void drft::GameState::setupActionMap()
{
	ActionMap& actions = getContext().actions;
	actions.bind("gameplay", "gameplay", "zoom_in",		[this]() {system::getCurrentCamera(getContext().registry).zoomIn();});
	actions.bind("gameplay", "gameplay", "zoom_out",	[this]() {system::getCurrentCamera(getContext().registry).zoomOut();});
	actions.bind("gameplay", "menu", "exit",			[this]() {requestStackPush(States::Pause);});
}

bool drft::GameState::handleEvent(const sf::Event& ev)
{
	if (_gameStateStack.handleEvent(ev)) return true;

	if (const auto keypressed = ev.getIf<sf::Event::KeyPressed>())
	{
		ModifiedInput input = KeybindingUtils::getModifiedInput(keypressed->scancode);
		if (getContext().actions.call("gameplay", "menu", input)) return true;
		if (getContext().actions.call("gameplay", "gameplay", input)) return true;

		_inputBuffer.press(input);
	}

	if (const auto keyreleased = ev.getIf<sf::Event::KeyReleased>())
	{
		ModifiedInput input = KeybindingUtils::getModifiedInput(keyreleased->scancode);
		_inputBuffer.release(input);
	}

	if (const auto mousepressed = ev.getIf<sf::Event::MouseButtonPressed>())
	{
		ModifiedInput input = KeybindingUtils::getModifiedInput(mousepressed->button);
		_inputBuffer.press(input);
	}

	if (const auto mousereleased = ev.getIf<sf::Event::MouseButtonReleased>())
	{
		ModifiedInput input = KeybindingUtils::getModifiedInput(mousereleased->button);
		_inputBuffer.release(input);
	}

	if (const auto mousescroll = ev.getIf<sf::Event::MouseWheelScrolled>())
	{
		if (mousescroll->delta > 0.5f)
		{
			getContext().actions.call("gameplay", "gameplay", "zoom_in");
		}
		else if (mousescroll->delta < -0.5f)
		{
			getContext().actions.call("gameplay", "gameplay", "zoom_out");
		}
	}

	return false;
}

bool drft::GameState::update()
{
	_inputBuffer.update();
	_gameStateStack.update();
	return true;
}

void drft::GameState::render(sf::RenderTarget& target)
{	
	_gameStateStack.render(target);
}

void drft::GameState::guiRender(sf::RenderTarget& target)
{
	_gameStateStack.guiRender(target);
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
