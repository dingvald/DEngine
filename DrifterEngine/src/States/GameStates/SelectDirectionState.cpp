#include "pch.h"
#include "SelectDirectionState.h"

#include <Actions/ActionMap.h>
#include <Defines/CardinalDirections.h>
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Components/Components.h"
#include <Components/PositionComponent.h>
#include "Components/RenderComponent.h"
#include <Utility/StandardLogger.h>
#include "Systems/Helpers/SpawnEffect.h"

using namespace entt::literals;

drft::SelectDirectionState::SelectDirectionState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	ActionMap& actions = getContext().actions;

    actions.bind("select_direction", "gameplay", "move_south_west",		[this]() { selectDirection(CardinalDirection::SouthWest); });
    actions.bind("select_direction", "gameplay", "move_south",			[this]() { selectDirection(CardinalDirection::South); });
    actions.bind("select_direction", "gameplay", "move_south_east",		[this]() { selectDirection(CardinalDirection::SouthEast); });
    actions.bind("select_direction", "gameplay", "move_west",			[this]() { selectDirection(CardinalDirection::West); });
    actions.bind("select_direction", "gameplay", "move_east",			[this]() { selectDirection(CardinalDirection::East); });
    actions.bind("select_direction", "gameplay", "move_north_west",		[this]() { selectDirection(CardinalDirection::NorthWest); });
    actions.bind("select_direction", "gameplay", "move_north",			[this]() { selectDirection(CardinalDirection::North); });
    actions.bind("select_direction", "gameplay", "move_north_east",		[this]() { selectDirection(CardinalDirection::NorthEast); });

	actions.bind("select_direction", "menu", "exit", [this]() { requestStackPop(); });
}

bool drft::SelectDirectionState::handleEvent(const sf::Event& ev)
{
	if (const auto keypressed = ev.getIf<sf::Event::KeyPressed>())
	{
		const ModifiedInput input = KeybindingUtils::getModifiedInput(keypressed->scancode);
		if (getContext().actions.call("select_direction", "menu", input)) return true;
		return getContext().actions.call("select_direction", "gameplay", input);
	}
	return false;
}

void drft::SelectDirectionState::onPush()
{
	auto selectDirectionView = getContext().registry.view<component::action::SelectDirection>();
	if (selectDirectionView.empty())
	{
		LOG_ERROR("Entered 'SelectDirection' state, but no entity with component");
		requestStackPop();
		return;
	}
	_sessionEntity = { getContext().registry, selectDirectionView.front() };

	placeTargetingTiles();
}

void drft::SelectDirectionState::onPop()
{
	getContext().registry.clear<component::action::SelectDirection>();
	getContext().registry.destroy(_targetingTileEntities.begin(), _targetingTileEntities.end());
}

void drft::SelectDirectionState::placeTargetingTiles()
{
	auto position = _sessionEntity.try_get<PositionComponent>();
	if (!position) return;

	auto surroundingTiles = spatial::getAdjacentPoints(spatial::toXY(position->tile));

	SpriteOptions radiusEffect = {
		.uvCoords = sf::Vector2i{0, 0},
		.texture = "rectangle"_hs,
		.uvSize = DefaultTileTextureSize,
		.layer = RenderLayer::Tiles,
		.color = sf::Color(0, 0, 100, 100)
	};
	for (auto&& tile : surroundingTiles)
	{
		auto effect = system::spawnEffect(getContext().registry,
			{
				.frames = {radiusEffect},
				.position = spatial::asTileSpace(tile),
				.ttl = -1
			});
		_targetingTileEntities.push_back(effect);
	}
}

void drft::SelectDirectionState::selectDirection(sf::Vector2i direction)
{
	if (auto selectDirection = _sessionEntity.try_get<component::action::SelectDirection>())
	{
		selectDirection->onDirectionSelect(direction);
	}
	else
	{
		LOG_ERROR("Entered 'SelectDirection' state, but no entity with component");
	}
	requestStackPop();
}
