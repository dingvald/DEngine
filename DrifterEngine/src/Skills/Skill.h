#pragma once

enum class SkillCategory
{
	Core,
	General,
	Combat,
	Weapon
};

enum class GainExpResult
{
	NoLevelUp,
	LevelUp
};

class Skill
{
public:
	static inline const int DefaultLevel = 1;

	Skill() = default;
	Skill(const char* skillName, int level);
	const std::string& name() const;
	const std::string& description() const;
	const SkillCategory category() const;

	int level() const;
	// Add exp to the skill
	// Returns true if the skill leveled up as a result
	GainExpResult gainExp(int amount);
	float getProgress() const;

private:
	std::string _name;
	int _level = DefaultLevel;
	int _exp = 0;
	int _expUntilNext = 0;

private:
	friend class cereal::access;
	template<typename Archive>
	void serialize(Archive& ar)
	{
		ar(_name, _level, _exp, _expUntilNext);
	}
};