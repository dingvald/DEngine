#include "pch.h"
#include "PowerAttackAbility.h"
#include <Skills/SkillIds.h>
#include <Defines/CommonGuiColors.h>
#include <Systems/Core/ActorSystem.h>
#include <Systems/Core/TweeningSystem.h>

#include <Components/PositionComponent.h>
#include <Components/TweeningComponent.h>
#include <Components/IncomingForceComponent.h>
#include <Components/HealthComponent.h>
#include <Components/Wrappers/BodyWrapper.h>

#include <Systems/Helpers/GetDominantSide.h>
#include <Systems/Helpers/EasingFunctions.h>
#include <Systems/Gameplay/SkillsSystem.h>
#include <Spatial/Conversions.h>
#include <Spatial/Helpers.h>
#include "Spatial/WorldGrid.h"
#include <Systems/Helpers/CalculateForceGenerated.h>
#include <Utility/EntityAccessors/GetItemInDominantHand.h>
#include <Utility/TGUIHelpers.h>

using namespace entt::literals;

namespace drft
{
	AbilityTargetingType PowerAttackAbility::getTargetingType() const
	{
		return AbilityTargetingType::SelectDirection;
	}

	bool PowerAttackAbility::isValid(entt::const_handle actor) const
	{
		return hasResources(actor);
	}

	void PowerAttackAbility::perform(entt::handle actor, std::optional<TilePosition> target) const
	{
		if (!target) return;

		const auto& position = actor.get<PositionComponent>();

		Tween moveToTween = {
			.targetOffset = drft::spatial::toFloatSpace(target.value() - position.tile) * 0.8f,
			.time = 0.35f,
			.easing = Easing::easeInBackSlow,
			.onFinish = [this, targetPosition = target.value()](entt::handle entity) { onTweenReachedTarget(entity, targetPosition); }
		};
		Tween moveBackTween = {
			.targetOffset = {0.f, 0.f, 0.f},
			.time = 0.50f,
			.easing = Easing::easeOutBack,
			.onFinish = [this](entt::handle entity) { onTweenReturnedToStart(entity); }
		};

		drft::system::TweeningSystem::tween(actor, moveToTween);
		drft::system::TweeningSystem::tween(actor, moveBackTween);

		spendResources(actor);
		drft::system::ActorSystem::setActionInProgress(actor);
	}

	int PowerAttackAbility::getTimeCost() const
	{
		return 150;
	}

	drft::IAbility::ResourceCosts drft::PowerAttackAbility::getResourceCosts(entt::const_handle) const
	{
		return { {AbilityResourceType::Stamina, 10} };
	}

	math::Range<int> PowerAttackAbility::getRange(entt::const_handle actor) const
	{
		return { 1, 1 };
	}

	std::vector<sf::Vector2i> PowerAttackAbility::getTargetingShape(entt::const_handle actor) const
	{
		return {};
	}

	drft::AbilityIconData PowerAttackAbility::getIconData() const
	{
		return AbilityIconData{
			.textureId = "icons"_hs,
			.color = guiColor::SofterRed,
			.uv = {4, 0}
		};
	}

	entt::id_type PowerAttackAbility::getAssociatedSkill() const
	{
		return SkillId::Strength;
	}

	std::string PowerAttackAbility::getContextualDescription(entt::const_handle actor) const
	{
		auto itemInHand = drft::util::getItemInDominantHand(actor);
		return std::format("A powerful attack using all your strength. Generates {} force.", 
			GuiHelpers::colorizedString(std::format("{:.1f}", calculatePowerAttackForce(actor, itemInHand)), guiColor::VariableGreen));
	}

	void PowerAttackAbility::onTweenReachedTarget(entt::handle actor, drft::TilePosition targetPosition) const
	{
		if (!actor) return;

		const auto& grid = actor.registry()->ctx().get<spatial::WorldGrid&>();
		const auto& positionComponent = actor.get<PositionComponent>();

		auto hasHealthFilter = [this, actor](entt::entity entity) -> bool { return actor.registry()->all_of<HealthComponent>(entity); };
		auto targets = grid.entitiesAt(targetPosition, hasHealthFilter);

		float force = 0.f;
		BodyWrapper body = { actor };
		auto itemInHand = body.getItemInDominantHand();
		if (!targets.empty())
		{
			force = calculatePowerAttackForce(actor, itemInHand);
			drft::system::SkillsSystem::useSkill(SkillId::Strength, (int)(std::sqrtf(force) * 10), actor);
		}

		for (auto&& target : targets)
		{
			actor.registry()->emplace_or_replace<IncomingForceComponent>(target, force, itemInHand);
		}
	}

	void PowerAttackAbility::onTweenReturnedToStart(entt::handle entity) const
	{
		drft::system::ActorSystem::setActionComplete(entity, drft::system::ActionCategory::Act);
	}

	float PowerAttackAbility::calculatePowerAttackForce(entt::const_handle actor, entt::const_handle item) const
	{
		GeneratedForce generatedForce = calculateForceGenerated(actor, item);
		return generatedForce.fromStrength * 3.3f;
	}
}