#include "pch.h"
#include "SkillsWrapper.h"

const std::vector<Skill>& drft::SkillsWrapper::getSkills() const
{
    if (auto component = tryGetUnderlyingConst())
    {
        return component->skills;
    }
    return {};
}
