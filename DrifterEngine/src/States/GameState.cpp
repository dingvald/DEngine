#include "pch.h"
#include "GameState.h"

#include <Engine/CommonEngineDirectories.h>
#include <Actions/ActionMap.h>

#include "Events/RequestStateChange.h"
#include <Keybindings/KeybindingsUtils.h>

#include <Universe/SolarSystem/SolarSystem.h>

#include <States/GameStates/CraftingState.h>
#include <States/GameStates/GameOverState.h>
#include <States/GameStates/InventoryState.h>
#include <States/GameStates/SelectDirectionState.h>
#include <States/GameStates/SelectTargetState.h>
#include <States/GameStates/SimulationState.h>
#include <States/GameStates/SkillsScreenState.h>

#include <Systems/Helpers/GetCurrentCamera.h>

#include <JSON/JSONHelpers.h>
#include "Utility/StandardLogger.h"
#include "Factory/EntityFactory.h"

#include <Generation/Decorators/DecoratorFactory.h>
#include <Generation/PositionSelector/PositionSelectorFactory.h>
#include <Generation/GenerationFinalizationContext.h>

static const float SCROLL_WHEEL_THRESHOLD = 0.5f;

drft::GameState::GameState(StateStack& stack, StateContext& context)
	: State(stack, context)
	, _gameStateStack(context)
{
	std::cout << "Initializing GameState..." << std::endl;

	loadGenerationRegistries();

	_solarSystem = std::make_unique<SolarSystem>(_generationRegistries);
	_dispatcher = std::make_unique<entt::dispatcher>();

	registerGameStates();
	connectEventHandlers();
	setupRegistryContext();
	setupActionMap();

	loadOrCreateUniverseGenerator();

	_gameStateStack.pushState(drft::States::Simulation);
}

void drft::GameState::registerGameStates()
{
	_gameStateStack.registerState<SimulationState>(States::Simulation);
	_gameStateStack.registerState<InventoryState>(States::Inventory);
	_gameStateStack.registerState<CraftingState>(States::Crafting);
	_gameStateStack.registerState<SelectDirectionState>(States::SelectDirection);
	_gameStateStack.registerState<SelectTargetState>(States::SelectTarget);
	_gameStateStack.registerState<SkillsScreenState>(States::Skills);
	_gameStateStack.registerState<GameOverState>(States::GameOver);
}

void drft::GameState::connectEventHandlers()
{
	_dispatcher->sink<events::RequestStateStackPush>().connect<&GameState::onRequestStatePush>(this);
}

void drft::GameState::loadOrCreateUniverseGenerator()
{
	LOG_MSG("Generating Universe...");

	json::JsonFileWrapper json{ SOLAR_SYSTEM_FILE_PATH, "solar_system" };
	if (!json.load())
	{
		LOG_ERROR("{} could not be loaded", SOLAR_SYSTEM_FILE_PATH.string());
		return;
	}
	else
	{
		_solarSystem->createFromJson(json.getRoot());
	}
	
	LOG_MSG("Universe generated.");
}

void drft::GameState::loadGenerationRegistries()
{
	LOG_MSG("Loading generation registries...");
	DecoratorFactory decorators;
	bindDecorators(decorators);

	PositionSelectorFactory positionSelectors;
	bindPositionSelectors(positionSelectors);

	PositionGeneratorFactory positionGenerators;
	bindPositionGenerators(positionGenerators);

	_generationRegistries.entityFactory.loadPrototypes(ENTITIES_DIRECTORY);
	_generationRegistries.biomes.loadBiomes(BIOMES_DIRECTORY);
	_generationRegistries.features.loadFeatures(BIOME_FEATURES_DIRECTORY, decorators);
	_generationRegistries.entityPacks.loadEntityPacks(ENTITY_PACKS_DIRECTORY);
	_generationRegistries.layerPacks.loadLayerPacks(LAYER_PACKS_DIRECTORY);
	_generationRegistries.prefabs.loadPrefabs(PREFABS_DIRECTORY);
	_generationRegistries.structures.loadStructures(STRUCTURES_DIRECTORY);
	LOG_MSG("Generation registries loaded.");

	// Assign factories to registries
	_generationRegistries.positionGenerators = std::move(positionGenerators);

	GenerationFinalizationContext finalizationContext{ _generationRegistries, decorators, positionSelectors };

	// Finalize
	LOG_MSG("Finalizing generation registries...");
	_generationRegistries.structures.finalize(finalizationContext);
	LOG_MSG("Generation registries finalized.");
}

void drft::GameState::setupRegistryContext()
{
	using namespace entt::literals;

	getContext().registry.ctx().emplace<system::InputBuffer&>(_inputBuffer);
	getContext().registry.ctx().emplace<Keybindings&>(getContext().keybindings);
	getContext().registry.ctx().emplace<IChunkDataSourceProvider*>(_solarSystem.get());
	getContext().registry.ctx().emplace<sf::RenderWindow&>(getContext().window);
	getContext().registry.ctx().emplace<TextureAtlas&>(getContext().textures);
	getContext().registry.ctx().emplace<const ControlsContext&>(getContext().controls);
	getContext().registry.ctx().emplace<ActionMap>(getContext().actions);
	getContext().registry.ctx().emplace_as<sf::Font&>("terminus"_hs, getContext().fonts.get("Terminus"));
	getContext().registry.ctx().emplace<EntityFactory&>(_generationRegistries.entityFactory);
	getContext().registry.ctx().emplace<entt::dispatcher&>(*_dispatcher);
	getContext().registry.ctx().emplace<tgui::Gui&>(getContext().gui);
}

void drft::GameState::setupActionMap()
{
	ActionMap& actions = getContext().actions;
	actions.bind("gameplay", "gameplay",	"zoom_in",		[this]() { system::getCurrentCamera(getContext().registry).zoomIn(); });
	actions.bind("gameplay", "gameplay",	"zoom_out",		[this]() { system::getCurrentCamera(getContext().registry).zoomOut(); });
	actions.bind("gameplay", "menu",		"exit",			[this]() { requestStackPush(States::Pause); });
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
		if (mousescroll->delta > SCROLL_WHEEL_THRESHOLD)
		{
			getContext().actions.call("gameplay", "gameplay", "zoom_in");
		}
		else if (mousescroll->delta < -SCROLL_WHEEL_THRESHOLD)
		{
			getContext().actions.call("gameplay", "gameplay", "zoom_out");
		}
	}

	return false;
}

bool drft::GameState::update(const float dt)
{
	_inputBuffer.update(dt);
	_gameStateStack.update(dt);

	if (_gameStateStack.isEmpty())
	{
		requestStackClear();
		requestStackPush(States::MainMenu);
	}

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
