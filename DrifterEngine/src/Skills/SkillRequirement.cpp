#include "pch.h"
#include "SkillRequirement.h"

SkillRequirement::SkillRequirement(entt::id_type skill, int minLevel)
	: _skillId(skill)
	, _minLevel(minLevel)
{
}

entt::id_type SkillRequirement::getSkillId() const
{
	return _skillId;
}

int SkillRequirement::getLevelRequirement() const
{
	return _minLevel;
}
