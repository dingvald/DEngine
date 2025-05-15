#include "pch.h"
#include "SkillsSystem.h"
#include <Events/SendFloatingMessageEvent.h>
#include <Components/PositionComponent.h>
#include <Components/SkillsComponent.h>
#include <Spatial/Helpers.h>
#include <Spatial/Conversions.h>

namespace Internal
{
	struct RegisterLevelUpHandlerEvent
	{
		const char* skill = nullptr;
		std::function<void(int, entt::handle)> handler;
	};

	static Skill* tryFindSkill(entt::id_type skillId, SkillsComponent& skillsComponent)
	{
		for (auto&& skill : skillsComponent.skills)
		{
			if (skill.id() == skillId)
			{
				return &skill;
			}
		}
		return nullptr;
	}

	static const Skill* tryFindSkill(entt::id_type skillId, const SkillsComponent& skillsComponent)
	{
		for (auto&& skill : skillsComponent.skills)
		{
			if (skill.id() == skillId)
			{
				return &skill;
			}
		}
		return nullptr;
	}
}


void drft::system::SkillsSystem::init()
{
	_dispatcher.sink<UseSkillEvent>().connect<&SkillsSystem::onUseSkillEvent>(this);
	_dispatcher.sink<Internal::RegisterLevelUpHandlerEvent>().connect<&SkillsSystem::onRegisterLevelUpHandler>(this);
}

void drft::system::SkillsSystem::onUseSkillEvent(UseSkillEvent& ev) const
{
	if (!ev.entity.all_of<SkillsComponent>()) return;

	auto& skillsComponent = ev.entity.get<SkillsComponent>();
	auto skillPtr = Internal::tryFindSkill(ev.skill, skillsComponent);
	if (!skillPtr) return;
	
	addExp(*skillPtr, ev.magnitude, ev.entity);
}

void drft::system::SkillsSystem::addExp(Skill& skill, int exp, entt::handle entity) const
{
	GainExpResult result = skill.gainExp(exp);
	if (result != GainExpResult::LevelUp) return;

	onSkillLevelUp(skill, entity);
}

void drft::system::SkillsSystem::onSkillLevelUp(const Skill& skill, entt::handle entity) const
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
	if (!_levelUpHandlers.contains(skill.name())) return;

	for (auto&& handler : _levelUpHandlers.at(skill.name()))
	{
		handler(skill.level(), entity);
	}
}

void drft::system::SkillsSystem::onRegisterLevelUpHandler(Internal::RegisterLevelUpHandlerEvent& ev)
{
	std::string skillName = ev.skill;
	_levelUpHandlers[skillName].push_back(ev.handler);
}

int drft::system::SkillsSystem::getSkillLevel(const char* skillName, entt::const_handle entity)
{
	if (!entity) return Skill::DefaultLevel;

	if (auto skillComponentPtr = entity.try_get<SkillsComponent>())
	{
		entt::id_type skillId = entt::hashed_string{ skillName };
		auto skill = Internal::tryFindSkill(skillId, *skillComponentPtr);
		if (!skill) return Skill::DefaultLevel;

		return skill->level();
	}
	return Skill::DefaultLevel;
}

void drft::system::SkillsSystem::useSkill(const char* skill, int magnitude, entt::handle entity)
{
	if (!entity) return;

	UseSkillEvent ev = { entity, entt::hashed_string{skill}, magnitude};

	entity.registry()->ctx().get<entt::dispatcher>().trigger(ev);
}

void drft::system::SkillsSystem::registerLevelUpHandler(const char* skill, LevelUpHandler handler, entt::registry& registry)
{
	Internal::RegisterLevelUpHandlerEvent ev;
	ev.skill = skill;
	ev.handler = handler;
	registry.ctx().get<entt::dispatcher>().trigger(ev);
}
