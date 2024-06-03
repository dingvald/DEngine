#include "pch.h"
#include "PlayerInput.h"
#include "Components/Components.h"
#include "Components/Actions/MoveAction.h"
#include "Components/SprintingComponent.h"
#include "Components/PlayerComponent.h"

#include "Components/Tags.h"
#include "Systems/HelperClasses/InputBuffer.h"
#include "Systems/Helpers/ToHotbarIndex.h"

static constexpr unsigned int INPUT_BUFFER_MAX_SIZE = 2;
static constexpr float REFRACTORY_PERIOD = 0.2f; // sec
static constexpr float HOLD_TIME = 0.5f; // sec

void drft::system::PlayerInput::init()
{
	using Key = sf::Keyboard;

	_actionMap.addAction(Key::Numpad1, [](entt::handle entity) {
		entity.emplace<PerformMoveAction>(sf::Vector2i(-1, 1)); 
		});
	_actionMap.addAction(Key::Numpad2, [](entt::handle entity) {
		entity.emplace<PerformMoveAction>(sf::Vector2i(0, 1)); 
		});
	_actionMap.addAction(Key::Numpad3, [](entt::handle entity) {
		entity.emplace<PerformMoveAction>(sf::Vector2i(1, 1)); 
		});
	_actionMap.addAction(Key::Numpad4, [](entt::handle entity) {
		entity.emplace<PerformMoveAction>(sf::Vector2i(-1, 0)); 
		});
	_actionMap.addAction(Key::Numpad6, [](entt::handle entity) {
		entity.emplace<PerformMoveAction>(sf::Vector2i(1, 0)); 
		});
	_actionMap.addAction(Key::Numpad7, [](entt::handle entity) {
		entity.emplace<PerformMoveAction>(sf::Vector2i(-1, -1)); 
		});
	_actionMap.addAction(Key::Numpad8, [](entt::handle entity) {
		entity.emplace<PerformMoveAction>(sf::Vector2i(0, -1)); 
		});
	_actionMap.addAction(Key::Numpad9, [](entt::handle entity) {
		entity.emplace<PerformMoveAction>(sf::Vector2i(1, -1)); 
		});
	_actionMap.addAction(Key::Numpad5, [](entt::handle entity) {
		entity.emplace<component::action::Wait>(); 
		});
	_actionMap.addAction(Key::G, [](entt::handle entity) {
		entity.emplace<component::action::PickUp>(); 
		});
	_actionMap.addAction(Key::E, [](entt::handle entity) {
		entity.emplace<component::action::OpenEquipment>();
		});
	_actionMap.addAction(Key::M, [](entt::handle entity) {
		entity.emplace<component::action::OpenWorldMap>();
		});
	_actionMap.addAction(Key::C, [](entt::handle entity) {
		entity.emplace<component::action::OpenCrafting>();
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
		entity.emplace<component::action::TryInteract>();
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

void drft::system::PlayerInput::update(const float dt)
{
	auto& inputBuffer = _registry->ctx().get<InputBuffer&>();
	if (!inputBuffer.isEmpty())
	{
		const auto key = inputBuffer.popKey();
		if (_actionMap.contains(key))
		{
			_keyState[key].active = false;
			if (_keyState[key].timeHeld <= std::numeric_limits<float>::epsilon())
			{
				// Just pressed
				_keyState[key].active = true;
				_keyState[key].timeHeld = 0.0f;
			}
			else if (_keyState[key].timeHeld >= HOLD_TIME)
			{
				// Held key long enough
				_keyState[key].active = true;
				_keyState[key].timeHeld -= REFRACTORY_PERIOD;
			}

			_keyState[key].timeHeld = std::min(_keyState[key].timeHeld + dt, HOLD_TIME);

			if (_keyState[key].active && _bufferedActions.size() < INPUT_BUFFER_MAX_SIZE)
			{
				_bufferedActions.push(_actionMap[key]);
			}

			for (auto& [otherKey, keyState] : _keyState)
			{
				if (otherKey == key) continue;
				keyState.timeHeld = 0.0f;
				keyState.active = false;
			}
		}
	}
	else
	{
		for (auto& [otherKey, keyState] : _keyState)
		{
			keyState.timeHeld = 0.0f;
			keyState.active = false;
		}
	}

	auto turnView = _registry->view<PlayerComponent, component::tag::CurrentActor>();
	for (auto entity : turnView)
	{
		if (_bufferedActions.empty()) continue;
		_bufferedActions.front()({ *_registry, entity });
		_bufferedActions.pop();
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


