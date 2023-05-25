#include "pch.h"
#include "WaitingSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"

void drft::system::WaitingSystem::init()
{
}

void drft::system::WaitingSystem::update(const float dt)
{
	auto waitView = registry->view<component::action::Wait, component::Position, component::tag::Active>();
	for (auto [entity, pos] : waitView.each())
	{
		registry->emplace_or_replace<component::action::ConsumeStamina>(entity, -2);
		registry->emplace_or_replace<component::action::SpendPoints>(entity, 100);
	}
}

void drft::system::WaitingSystem::onUpdateEnd()
{
	registry->clear<component::action::Wait>();
}
