#include "pch.h"
#include "SimulationState.h"

#include <Engine/CommonEngineDirectories.h>

#include <Factory/EntityFactory.h>
#include <Components/PositionComponent.h>
#include <Components/CameraTargetComponent.h>
#include <Keybindings/Keybindings.h>
#include <Spatial/WorldGrid.h>

#include "Systems/SystemScheduler.h"

#include <Systems/Actions/AbilityActionSystem.h>
#include "Systems/Actions/SprintingSystem.h"
#include "Systems/Actions/DropItemSystem.h"
#include "Systems/Actions/EquipItemSystem.h"
#include "Systems/Actions/CraftItemSystem.h"
#include "Systems/Actions/HotbarActionSystem.h"
#include <Systems/Actions/MouseActionSystem.h>
#include "Systems/Actions/MoveActionSystem.h"
#include "Systems/Actions/WaitActionSystem.h"
#include "Systems/Actions/PickUpSystem.h"
#include "Systems/Actions/InteractionSystem.h"
#include "Systems/Actions/MeleeAttackActionSystem.h"

#include "Systems/Core/HUD.h"
#include "Systems/Core/RealityBubble.h"
#include "Systems/Core/PlayerInput.h"
#include "Systems/Core/ArtificialInput.h"
#include <Systems/Core/AiSystem.h>
#include "Systems/Core/Camera.h"
#include <Systems/Core/MouseStateSystem.h>
#include <Systems/Core/MouseVisualizationSystem.h>
#include "Systems/Core/ChunkManager.h"
#include "Systems/Core/ItemUniqueIDGenerator.h"
#include "Systems/Core/ActorSystem.h"
#include <Systems/Core/TweeningSystem.h>
#include "Systems/Core/WorldGridResolver.h"
#include "Systems/Core/FloatingTextSystem.h"
#include "Systems/Core/VisualEffectSystem.h"

#include "Systems/Rendering/CullingSystem.h"
#include "Systems/Rendering/AnimationSystem.h"
#include "Systems/Rendering/SyncedAnimationSystem.h"
#include "Systems/Rendering/PlayerFOVSystem.h"
#include "Systems/Rendering/EntityRenderer.h"
#include "Systems/Rendering/LightingSystem.h"
#include "Systems/Rendering/SpriteControllerSystem.h"

#include "Systems/Gameplay/BodyPartSystem.h"
#include "Systems/Gameplay/CollisionSystem.h"
#include "Systems/Gameplay/HealthSystem.h"
#include "Systems/Gameplay/StaminaSystem.h"
#include "Systems/Gameplay/DayNightCycleSystem.h"
#include "Systems/Gameplay/DeathSystem.h"
#include "Systems/Gameplay/ProjectileSystem.h"
#include "Systems/Gameplay/PathNavSystem.h"
#include "Systems/Gameplay/HealingSystem.h"
#include "Systems/Gameplay/OpenableSystem.h"
#include "Systems/Gameplay/ConsumableSystem.h"
#include "Systems/Gameplay/LightSourceSystem.h"
#include "Systems/Gameplay/LiquidSystem.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Systems/Gameplay/LevelingSystem.h"
#include "Systems/Gameplay/TickingLifetimeSystem.h"
#include "Systems/Gameplay/DetermineCraftableItemsSystem.h"

#include "Systems/PlayerSpecific/OpenEquipmentSystem.h"
#include "Systems/PlayerSpecific/OpenCraftingSystem.h"
#include "Systems/PlayerSpecific/SelectDirectionSystem.h"
#include "Systems/PlayerSpecific/TargetSelectSystem.h"

#include "Utility/SaveEntity.h"
#include "Utility/LoadEntity.h"
#include "Utility/SaveRegistry.h"
#include "Utility/LoadRegistry.h"

drft::SimulationState::SimulationState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	_systems = std::make_unique<system::SystemScheduler>(getContext().registry);

	setupRegistryContext();
	importSystems();
	loadRegistry();
	loadPlayer();

	std::cout << "Starting Simulation..." << std::endl;
	_systems->startAll();
	std::cout << "<<< Simulation Started >>>" << std::endl;
}

bool drft::SimulationState::handleEvent(const sf::Event& ev)
{
    return false;
}

bool drft::SimulationState::update()
{
	_systems->update();
    return true;
}

void drft::SimulationState::render(sf::RenderTarget& target)
{
	_systems->render(target);
}

void drft::SimulationState::onPop()
{
	savePlayer();
	_systems->shutdownAll();
	saveRegistry();
}

void drft::SimulationState::onEnter()
{
	system::MouseVisualizationSystem::changeMouseVisibility(getContext().registry, { true, true });
	system::HUD::setEnabled(getContext().registry, true);
}

void drft::SimulationState::onExit()
{
	system::MouseVisualizationSystem::changeMouseVisibility(getContext().registry, { false, false });
	system::HUD::setEnabled(getContext().registry, false);
}

void drft::SimulationState::setupRegistryContext()
{
	getContext().registry.ctx().emplace<spatial::WorldGrid&>(_world);
	getContext().registry.ctx().emplace<Keybindings&>(getContext().keybindings);
}

void drft::SimulationState::importSystems()
{
	std::cout << "Importing Systems..." << std::endl;

	using namespace system;

	_systems->add<RealityBubble>();
	_systems->add<ActorSystem>();

	_systems->add<MouseStateSystem>();
	_systems->add<MouseVisualizationSystem>();
	_systems->add<PlayerInput>();
	_systems->add<ArtificialInput>();
	_systems->add<AiSystem>();

	_systems->add<PathNavSystem>();
	_systems->add<ProjectileSystem>();
	_systems->add<InteractionSystem>();
	_systems->add<WaitActionSystem>();
	_systems->add<AbilityActionSystem>();
	_systems->add<MouseActionSystem>();
	_systems->add<DropItemSystem>();
	_systems->add<EquipItemSystem>();
	_systems->add<CraftItemSystem>();
	_systems->add<BodyPartSystem>();
	_systems->add<HotbarActionSystem>();
	_systems->add<HealthSystem>();
	_systems->add<DeathSystem>();
	_systems->add<LevelingSystem>();
	_systems->add<ChunkManager>();
	_systems->add<TweeningSystem>();

	_systems->add<DayNightCycleSystem>();
	_systems->add<SprintingSystem>();
	_systems->add<LiquidSystem>();
	
	_systems->add<LightSourceSystem>();
	
	_systems->add<VisualEffectSystem>();
	_systems->add<SpriteControllerSystem>();
	_systems->add<AnimationSystem>();
	_systems->add<SyncedAnimationSystem>();

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
	_systems->add<PickUpSystem>();
	_systems->add<MoveActionSystem>();
	_systems->add<MeleeAttackActionSystem>();
	_systems->add<CollisionSystem>();
	_systems->add<StaminaSystem>();

	// Rendering Systems - Be mindful of the order
	_systems->add<Camera>();
	_systems->add<CullingSystem>();
	_systems->add<PlayerFOVSystem>();
	_systems->add<LightingSystem>();
	_systems->add<EntityRenderer>();
	_systems->add<FloatingTextSystem>();
	_systems->add<HUD>();

	_systems->initAll();
}

void drft::SimulationState::loadPlayer()
{
	if (std::filesystem::exists(PLAYER_SAVE_FILE_PATH))
	{
		_player = { getContext().registry, getContext().registry.create() };
		util::loadEntityFromFile(_player, PLAYER_SAVE_FILE_PATH);
	}
	else
	{
		const EntityFactory& factory = getContext().registry.ctx().get<const EntityFactory&>();
		assert(factory.has("Player"));
		_player = factory.build("Player", getContext().registry);
		_player.patch<PositionComponent>([](PositionComponent& pos)
			{
				pos.tile = { 1024, 1024, 0 };
			});
	}

	_player.emplace<CameraTargetComponent>();
}

void drft::SimulationState::savePlayer()
{
	if (!isPlayerAlive()) return;

	util::saveEntityToFile(_player, PLAYER_SAVE_FILE_PATH);
	_player.destroy();
	getContext().registry.compact();
}

void drft::SimulationState::loadRegistry()
{
	if (!std::filesystem::exists(MAIN_REGISTRY_FILE_PATH)) return;

	util::loadRegistryFromFile(getContext().registry, MAIN_REGISTRY_FILE_PATH);
}

void drft::SimulationState::saveRegistry()
{
	if (!std::filesystem::exists(PLAYER_SAVE_FILE_PATH)) return;

	util::saveRegistryToFile(getContext().registry, MAIN_REGISTRY_FILE_PATH);
}

bool drft::SimulationState::isPlayerAlive() const
{
	return _player.valid();
}
