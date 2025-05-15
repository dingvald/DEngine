#pragma once
#include <Components/Wrappers/ComponentWrapper.h>
#include <Components/SkillsComponent.h>
#include <Skills/Skill.h>

namespace drft
{
	class SkillsWrapper : public ComponentWrapper<SkillsComponent>
	{
	public:
		const std::vector<Skill>& getSkills() const;
	};
}