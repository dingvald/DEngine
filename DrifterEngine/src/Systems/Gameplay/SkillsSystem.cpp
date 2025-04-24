#include "pch.h"
#include "SkillsSystem.h"
#include <Events/SendFloatingMessageEvent.h>
#include <Components/PositionComponent.h>
#include <Spatial/Helpers.h>
#include <Spatial/Conversions.h>
#include <Utility/StringManipulation.h>


void drft::system::SkillsSystem::init()
{
	_dispatcher.sink<UseSkillEvent>().connect<&SkillsSystem::onUseSkillEvent>(this);
}

void drft::system::SkillsSystem::onUseSkillEvent(UseSkillEvent& ev) const
{
	if (!ev.entity.all_of<SkillsComponent>()) return;

	auto& skillsComponent = ev.entity.get<SkillsComponent>();
	auto& skill = skillsComponent.skills.at(ev.skill);
	
	addExp(skill, ev.magnitude, ev.entity);
}

void drft::system::SkillsSystem::addExp(Skill& skill, int exp, entt::handle entity) const
{
	GainExpResult result = skill.gainExp(exp);
	if (result != GainExpResult::LevelUp) return;

	onSkillLevelUp(skill, entity);
}

void drft::system::SkillsSystem::onSkillLevelUp(const Skill& skill, entt::const_handle entity) const
{
	if (auto position = entity.try_get<PositionComponent>())
	{
		events::SendFloatingMessageEvent message =
		{
			.message = std::format("{} increased to {}", skill.name(), skill.level()),
			.color = sf::Color{212, 175, 55},
			.position = spatial::toXY(spatial::toFloatSpace(position->tile)) - sf::Vector2f{0.f, 4.f},
			.velocity = {0,-0.2},
			.fades = true,
			.isScreenSpace = false,
			.ttl = 120
		};
		_dispatcher.trigger(message);
	}
}

int drft::system::SkillsSystem::getSkillLevel(const char* skill, entt::const_handle entity)
{
	if (!entity) return 0;

	if (auto skillComponent = entity.try_get<SkillsComponent>())
	{
		const entt::id_type hashedSkillName = entt::hashed_string{ skill };
		if (!skillComponent->skills.contains(hashedSkillName)) return 0;

		return skillComponent->skills.at(hashedSkillName).level();
	}
	return 0;
}

void drft::system::SkillsSystem::useSkill(const char* skill, int magnitude, entt::handle entity)
{
	if (!entity) return;

	UseSkillEvent ev = { entity, entt::hashed_string{skill}, magnitude};

	entity.registry()->ctx().get<entt::dispatcher>().trigger(ev);
}
