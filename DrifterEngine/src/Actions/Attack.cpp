#include "pch.h"
#include <cstring>
#include "Attack.h"
#include "Components/Components.h"

using namespace drft::action;

std::unique_ptr<Action> drft::action::Attack::execute(entt::registry& registry, const entt::entity actor)
{
	

	return nullptr;
}

std::unique_ptr<Action> drft::action::Attack::clone() const 
{
	return std::make_unique<Attack>(potentialTargets);
}