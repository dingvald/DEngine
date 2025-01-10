#include "pch.h"
#include "PlayerInput.h"
#include "Components/Components.h"
#include <Components/CurrentActorComponent.h>
#include "Components/Actions/MoveAction.h"
#include "Components/Actions/InteractionAction.h"
#include "Components/Actions/WaitAction.h"
#include "Components/SprintingComponent.h"
#include "Components/PlayerComponent.h"

#include "Components/Tags.h"
#include "Systems/HelperClasses/InputBuffer.h"
#include "Systems/Helpers/ToHotbarIndex.h"


void drft::system::PlayerInput::init()
{
	using Key = sf::Keyboard;

	_actionMap.addAction(Key::Numpad1, [](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(-1, 1));
		});
	_actionMap.addAction(Key::Numpad2, [](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(0, 1));
		});
	_actionMap.addAction(Key::Numpad3, [](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(1, 1));
		});
	_actionMap.addAction(Key::Numpad4, [](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(-1, 0));
		});
	_actionMap.addAction(Key::Numpad6, [](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(1, 0));
		});
	_actionMap.addAction(Key::Numpad7, [](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(-1, -1));
		});
	_actionMap.addAction(Key::Numpad8, [](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(0, -1));
		});
	_actionMap.addAction(Key::Numpad9, [](entt::handle entity) {
		entity.emplace_or_replace<MoveAction>(sf::Vector2i(1, -1));
		});
	_actionMap.addAction(Key::Numpad5, [](entt::handle entity) {
		entity.emplace_or_replace<WaitAction>();
		});
	_actionMap.addAction(Key::G, [](entt::handle entity) {
		entity.emplace_or_replace<component::action::PickUp>();
		});
	_actionMap.addAction(Key::E, [](entt::handle entity) {
		entity.emplace_or_replace<component::action::OpenEquipment>();
		});
	_actionMap.addAction(Key::C, [](entt::handle entity) {
		entity.emplace_or_replace<component::action::OpenCrafting>();
		});
	_actionMap.addAction(Key::S, [](entt::handle entity) {
			if (entity.all_of<SprintingComponent>())
			{
				entity.remove<SprintingComponent>();
			}
			else
			{
				entity.emplace<SprintingComponent>();
			}
		});
	_actionMap.addAction(Key::Space, [](entt::handle entity) {
		entity.emplace_or_replace<InteractionAction>();
		});

	// Hotbar //
	for (int i = 0; i < HOTBAR_SIZE; ++i)
	{
		_actionMap.addAction(static_cast<sf::Keyboard::Key>(static_cast<int>(Key::Num0) + i), 
			[i](entt::handle entity) {
			entity.emplace<component::action::HotbarPressed>(toHotbarIndex(i));
		});
	}
}

void drft::system::PlayerInput::update()
{
	auto& inputBuffer = _registry.ctx().get<InputBuffer&>();
	auto turnView = _registry.view<PlayerComponent, CurrentActorComponent>();
	for (auto&& [entity, player, currentActor] : turnView.each())
	{
		if (currentActor.state != CurrentActorState::Pending) continue;

		const auto key = inputBuffer.pop();
		if (!_actionMap.contains(key)) continue;

		_actionMap[key](entt::handle{ _registry, entity });
	}
}

void drft::system::ActionMap::addAction(sf::Keyboard::Key key, emplaceFunc func)
{
	_map[key] = func;
}

bool drft::system::ActionMap::contains(sf::Keyboard::Key key) const
{
	return _map.contains(key);
}

drft::system::ActionMap::emplaceFunc drft::system::ActionMap::operator[](sf::Keyboard::Key key)
{
	return _map.at(key);
}

std::unordered_map<sf::Keyboard::Key, drft::system::ActionMap::emplaceFunc>& drft::system::ActionMap::iterate()
{
	return _map;
}


