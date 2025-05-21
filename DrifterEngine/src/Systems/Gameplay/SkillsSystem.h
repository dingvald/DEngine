#pragma once

#include "Systems/System.h"

class Skill;

namespace Internal
{
	struct RegisterLevelUpHandlerEvent;
}

namespace drft::system
{
	class SkillsSystem : public System
	{
	public:
		using LevelUpHandler = std::function<void(int, entt::handle)>;
		using System::System;

		static int getSkillLevel(entt::id_type skill, entt::const_handle entity);
		static void useSkill(entt::id_type skill, int magnitude, entt::handle entity);
		static void registerLevelUpHandler(entt::id_type skill, LevelUpHandler handler, entt::registry& registry);

	private:
		struct UseSkillEvent
		{
			entt::handle entity;
			entt::id_type skill;
			int magnitude = 1;
		};
		void init() override;

		void onUseSkillEvent(UseSkillEvent& ev) const;

		void addExp(Skill& skill, int exp, entt::handle entity) const;
		void onSkillLevelUp(const Skill& skill, entt::handle entity) const;
		void onRegisterLevelUpHandler(Internal::RegisterLevelUpHandlerEvent& ev);

	private:
		using LevelUpHandlers = std::vector<LevelUpHandler>;
		std::unordered_map<entt::id_type, LevelUpHandlers> _levelUpHandlers;
	};
}