#include "pch.h"
#include "PlayerInput.h"

#include <Actions/ActionMap.h>

#include <Components/Actions/HotbarAction.h>
#include <Components/Actions/MeleeAttackAction.h>
#include <Components/Actions/MouseContextAction.h>
#include <Components/Actions/MouseInspectAction.h>
#include <Components/Actions/OpenSkillsScreenAction.h>
#include <Components/CurrentActorComponent.h>
#include <Components/PathNavComponent.h>
#include "Components/Actions/InteractionAction.h"
#include "Components/Actions/MoveAction.h"
#include "Components/Actions/WaitAction.h"
#include "Components/Components.h"
#include "Components/PlayerInputComponent.h"
#include "Components/SprintingComponent.h"

#include <Keybindings/Keybindings.h>
#include "Systems/HelperClasses/InputBuffer.h"
#include "Systems/Helpers/ToHotbarIndex.h"
#include <Systems/Helpers/RequestGameplayScreen.h>
#include <Utility/StandardLogger.h>


void drft::system::PlayerInput::init()
{
	ActionMap& actions = _registry.ctx().get<ActionMap>();

	actions.bind("player_input", "gameplay", "move_south_west",			[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(-1, 1));
		});
	actions.bind("player_input", "gameplay", "move_south",				[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(0, 1));
		});
	actions.bind("player_input", "gameplay", "move_south_east",			[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(1, 1));
		});
	actions.bind("player_input", "gameplay", "move_west",				[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(-1, 0));
		});
	actions.bind("player_input", "gameplay", "move_east",				[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(1, 0));
		});
	actions.bind("player_input", "gameplay", "move_north_west",			[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(-1, -1));
		});
	actions.bind("player_input", "gameplay", "move_north",				[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(0, -1));
		});
	actions.bind("player_input", "gameplay", "move_north_east",			[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(1, -1));
		});
	actions.bind("player_input", "gameplay", "wait",					[](entt::handle entity) {
		entity.emplace_or_replace<WaitAction>();
		});
											 
	actions.bind("player_input", "gameplay", "force_attack_south_west", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(-1, 1));
		});
	actions.bind("player_input", "gameplay", "force_attack_south",		[](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(0, 1));
		});
	actions.bind("player_input", "gameplay", "force_attack_south_east", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(1, 1));
		});
	actions.bind("player_input", "gameplay", "force_attack_west",		[](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(-1, 0));
		});
	actions.bind("player_input", "gameplay", "force_attack_east",		[](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(1, 0));
		});
	actions.bind("player_input", "gameplay", "force_attack_north_west", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(-1, -1));
		});
	actions.bind("player_input", "gameplay", "force_attack_north",		[](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(0, -1));
		});
	actions.bind("player_input", "gameplay", "force_attack_north_east", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(1, -1));
		});
											 
	actions.bind("player_input", "gameplay", "pick_up",					[](entt::handle entity) {
		entity.emplace_or_replace<component::action::PickUp>();
		});
	actions.bind("player_input", "gameplay", "open_equipment",			[](entt::handle entity) {
		requestGameplayScreen(drft::States::Inventory, entity);
		});
	actions.bind("player_input", "gameplay", "open_crafting",			[](entt::handle entity) {
		requestGameplayScreen(drft::States::Crafting, entity);
		});
	actions.bind("player_input", "gameplay", "open_skills",				[](entt::handle entity) {
		requestGameplayScreen(drft::States::Skills, entity);
		});
	actions.bind("player_input", "gameplay", "open_abilities",			[](entt::handle entity) {
		requestGameplayScreen(drft::States::Abilities, entity);
		});
	actions.bind("player_input", "gameplay", "toggle_sprint",			[](entt::handle entity) {
			if (entity.all_of<SprintingComponent>())
			{
				entity.remove<SprintingComponent>();
			}
			else
			{
				entity.emplace<SprintingComponent>();
			}
		});
	actions.bind("player_input", "gameplay", "interact",				[](entt::handle entity) {
		entity.emplace_or_replace<InteractionAction>();
		});
											 
	actions.bind("player_input", "gameplay", "mouse_contextual",		[](entt::handle entity) {
		entity.emplace_or_replace<MouseContextAction>();
		});
	actions.bind("player_input", "gameplay", "mouse_inspect",			[](entt::handle entity) {
		entity.emplace_or_replace<MouseInspectAction>();
		});

	// Hotbar //
	for (int i = 0; i < HOTBAR_SIZE; ++i)
	{
		actions.bind("player_input", "gameplay", std::format("hotbar_{}", i), [i](entt::handle entity) {
			entity.emplace_or_replace<HotbarAction>(fromHotbarIndex(i));
		});
	}
}

void drft::system::PlayerInput::update()
{
	auto& inputBuffer = _registry.ctx().get<InputBuffer>();
	auto& actions = _registry.ctx().get<ActionMap>();

	auto currentPlayerView = _registry.view<PlayerInputComponent, CurrentActorComponent>(entt::exclude<PathNavComponent>);
	for (auto&& [entity, player, currentActor] : currentPlayerView.each())
	{
		if (currentActor.state != CurrentActorState::Pending) continue;
		if (inputBuffer.isEmpty()) continue;

		entt::handle playerHandle = { _registry, entity };
		if (!playerHandle)
		{
			LOG_ERROR("From PlayerInput: Player handle invalid");
			continue;
		}

		ModifiedInput key = inputBuffer.pop();
		actions.call("player_input", "gameplay", key, playerHandle);
	}
}

