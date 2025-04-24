#pragma once

#include "Systems/System.h"
#include <Components/SkillsComponent.h>

namespace drft::system
{
	class SkillsSystem : public System
	{
	public:
		using System::System;

		static int getSkillLevel(const char* skill, entt::const_handle entity);
		static void useSkill(const char* skill, int magnitude, entt::handle entity);

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
		void onSkillLevelUp(const Skill& skill, entt::const_handle entity) const;
	};
}