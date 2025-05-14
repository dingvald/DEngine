#include "pch.h"
#include "SkillsWrapper.h"

std::vector<const Skill*> drft::SkillsWrapper::getSkills() const
{
    std::vector<const Skill*> result;
    if (auto component = tryGetUnderlyingConst())
    {
        for (auto&& [id, skill] : component->skills)
        {
            result.emplace_back(&skill);
        }
    }
    return result;
}
