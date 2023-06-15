#include "pch.h"
#include "DoorToggleSystem.h"
#include "Components/Components.h"
#include "Utility/SpriteIndexer.h"

void drft::system::DoorToggleSystem::init()
{
	registry->on_construct<component::action::ToggleDoor>().connect<&DoorToggleSystem::onContructDoorToggle>(this);
}

void drft::system::DoorToggleSystem::onUpdateEnd()
{
	registry->clear<component::action::ToggleDoor>();
}

void drft::system::DoorToggleSystem::onContructDoorToggle(entt::registry& registry, entt::entity entity)
{
	auto& door = registry.get<component::Door>(entity);
	if (door.isOpen)
	{
		auto& render = registry.get<component::Render>(entity);
		render.sprite -= 1;
		render.layer = 2;
		auto& physical = registry.get<component::Physical>(entity);
		physical.blocks = true;
		registry.emplace_or_replace<component::LightBlocking>(entity);
		door.isOpen = false;
	}
	else
	{
		auto& render = registry.get<component::Render>(entity);
		render.sprite += 1;
		render.layer = 1;
		auto& physical = registry.get<component::Physical>(entity);
		physical.blocks = false;
		registry.remove<component::LightBlocking>(entity);
		door.isOpen = true;
	}
}
