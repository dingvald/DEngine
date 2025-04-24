#include "pch.h"
#include "Skill.h"

namespace
{
    int determineAddedExpForLevel(int level)
    {
        return std::floorf(level + 300.0f * std::powf(2.0f, level / 7.f));
    }

    int determineExpForLevel(int level)
    {
        int total = 0;
        for (int l = 0; l <= level; l++)
        {
            total += determineAddedExpForLevel(l);
        }
        return total;
    }
}



namespace 
{
    using namespace entt::literals;
    struct SkillInfo
    {
        std::string name;
        std::string description;
        SkillCategory category;
    };
    class SkillInfoMap
    {
    public:
        static void addSkill(const char* name, const char* description, SkillCategory category)
        {
            _skillMap.emplace(entt::hashed_string{ name }, SkillInfo{ name, description, category });
        }
        static const SkillInfo& getSkill(const char* name)
        {
            entt::id_type hashedName = entt::hashed_string{ name };
            if (!_skillMap.contains(hashedName)) return _skillMap.at("NULL_SKILL"_hs);
            return _skillMap.at(hashedName);
        }

    private:
        static inline std::unordered_map<entt::id_type, SkillInfo> _skillMap;
    };
}



Skill::Skill(const char* skillName, int level)
    : _name(skillName)
    , _level(level)
{
    _expUntilNext = determineExpForLevel(_level);
}

const std::string& Skill::name() const
{
    return _name;
}

const std::string& Skill::description() const
{
    static const std::string EmptyString = {};
    return EmptyString;
}

const SkillCategory Skill::category() const
{
    return SkillCategory();
}

int Skill::level() const
{
    return _level;
}

GainExpResult Skill::gainExp(int amount)
{
    _exp += amount;
    if (_exp >= _expUntilNext)
    {
        _level++;
        _exp -= _expUntilNext;
        _expUntilNext += determineAddedExpForLevel(_level);
        return GainExpResult::LevelUp;
    }
    return GainExpResult::NoLevelUp;
}
