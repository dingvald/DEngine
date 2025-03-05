#include "pch.h"
#include "PlayerInput.h"
#include "Components/Components.h"
#include <Components/CurrentActorComponent.h>
#include <Components/Actions/MeleeAttackAction.h>
#include <Components/Actions/MouseContextAction.h>
#include <Components/Actions/MouseInspectAction.h>
#include "Components/Actions/MoveAction.h"
#include "Components/Actions/InteractionAction.h"
#include "Components/Actions/WaitAction.h"
#include "Components/SprintingComponent.h"
#include "Components/PlayerInputComponent.h"
#include <Components/PathNavComponent.h>

#include <Keybindings/Keybindings.h>
#include "Systems/HelperClasses/InputBuffer.h"
#include "Systems/Helpers/ToHotbarIndex.h"


void drft::system::PlayerInput::init()
{
	_actionMap.bindAction("move_south_west",	[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(-1, 1));
		});
	_actionMap.bindAction("move_south",			[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(0, 1));
		});
	_actionMap.bindAction("move_south_east",	[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(1, 1));
		});
	_actionMap.bindAction("move_west",			[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(-1, 0));
		});
	_actionMap.bindAction("move_east",			[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(1, 0));
		});
	_actionMap.bindAction("move_north_west",	[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(-1, -1));
		});
	_actionMap.bindAction("move_north",			[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(0, -1));
		});
	_actionMap.bindAction("move_north_east",	[](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(1, -1));
		});
	_actionMap.bindAction("wait",				[](entt::handle entity) {
		entity.emplace_or_replace<WaitAction>();
		});

	_actionMap.bindAction("force_attack_south_west", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(-1, 1));
		});
	_actionMap.bindAction("force_attack_south", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(0, 1));
		});
	_actionMap.bindAction("force_attack_south_east", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(1, 1));
		});
	_actionMap.bindAction("force_attack_west", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(-1, 0));
		});
	_actionMap.bindAction("force_attack_east", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(1, 0));
		});
	_actionMap.bindAction("force_attack_north_west", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(-1, -1));
		});
	_actionMap.bindAction("force_attack_north", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(0, -1));
		});
	_actionMap.bindAction("force_attack_north_east", [](entt::handle entity) {
		entity.emplace_or_replace<MeleeAttackAction>(sf::Vector2i(1, -1));
		});

	_actionMap.bindAction("pick_up",			[](entt::handle entity) {
		entity.emplace_or_replace<component::action::PickUp>();
		});
	_actionMap.bindAction("open_equipment",		[](entt::handle entity) {
		entity.emplace_or_replace<component::action::OpenEquipment>();
		});
	_actionMap.bindAction("open_crafting",		[](entt::handle entity) {
		entity.emplace_or_replace<component::action::OpenCrafting>();
		});
	_actionMap.bindAction("toggle_sprint",		[](entt::handle entity) {
			if (entity.all_of<SprintingComponent>())
			{
				entity.remove<SprintingComponent>();
			}
			else
			{
				entity.emplace<SprintingComponent>();
			}
		});
	_actionMap.bindAction("interact",			[](entt::handle entity) {
		entity.emplace_or_replace<InteractionAction>();
		});

	// Hotbar //
	for (int i = 0; i < HOTBAR_SIZE; ++i)
	{
		_actionMap.bindAction(std::format("hotbar_{}", i), [i](entt::handle entity) {
			entity.emplace<component::action::HotbarPressed>(toHotbarIndex(i));
		});
	}
}

void drft::system::PlayerInput::update()
{
	auto& inputBuffer = _registry.ctx().get<InputBuffer&>();
	auto& keybindings = _registry.ctx().get<Keybindings&>();

	auto currentPlayerView = _registry.view<PlayerInputComponent, CurrentActorComponent>(entt::exclude<PathNavComponent>);
	for (auto&& [entity, player, currentActor] : currentPlayerView.each())
	{
		if (currentActor.state != CurrentActorState::Pending) continue;

		entt::handle playerHandle = { _registry, entity };

		// TODO: Unifiy mouse input and keyboard input into single datastructure
		if (auto mouse = inputBuffer.popMouse())
		{
			if (mouse.value() == sf::Mouse::Button::Left)
			{
				playerHandle.emplace_or_replace<MouseContextAction>();
			}
			else if (mouse.value() == sf::Mouse::Button::Right)
			{
				playerHandle.emplace_or_replace<MouseInspectAction>();
			}
		}
		else
		{
			ModifiedKey key = inputBuffer.popKey();
			if (auto action = keybindings["gameplay"].getActionForKey(key))
			{
				_actionMap.callAction(action.value(), playerHandle);
			}
		}
	}
}

