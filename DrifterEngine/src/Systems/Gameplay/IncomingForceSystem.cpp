#include "pch.h"
#include "IncomingForceSystem.h"
#include <Components/BaseDamageComponent.h>
#include <Components/IncomingForceComponent.h>
#include <Components/SolidMaterialComponent.h>
#include <Components/LiquidMaterialComponent.h>
#include <Components/GasMaterialComponent.h>
#include <Components/Components.h>
#include <Systems/Helpers/MaterialForceDistribution.h>

// Solid Property Scaling Factors
static const float HardnessScalar = 4.f;
static const float DensityScalar = 3.f;
static const float BrittlenessScalar = 2.f;
static const float MalleabilityScalar = 1.f;
static const float SolidScalarSum = HardnessScalar + DensityScalar + BrittlenessScalar + MalleabilityScalar;

void drft::system::IncomingForceSystem::init()
{
	_random.setSeed(rng::GlobalSeed);
}

void drft::system::IncomingForceSystem::update(const float dt)
{
	auto solidview = _registry.view<IncomingForceComponent, SolidMaterialComponent>();
	for (auto&& [entity, incomingForce, solid] : solidview.each())
	{
		entt::handle hit = { _registry, entity };
		entt::handle other = { _registry, incomingForce.entity };
		
		hit.emplace_or_replace<component::action::TakeDamage>(incomingForce.force, other.entity());
	}

	auto liquidview = _registry.view<IncomingForceComponent, LiquidMaterialComponent>();
	for (auto&& [entity, incomingForce, liquid] : liquidview.each())
	{
		entt::handle hit = { _registry, entity };
		entt::handle other = { _registry, incomingForce.entity };
	}

	auto gasview = _registry.view<IncomingForceComponent, GasMaterialComponent>();
	for (auto&& [entity, incomingForce, gas] : gasview.each())
	{
		entt::handle hit = { _registry, entity };
		entt::handle other = { _registry, incomingForce.entity };
	}

	_registry.clear<IncomingForceComponent>();
}

