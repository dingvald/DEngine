#pragma once

class SkillRequirement
{
public:
	SkillRequirement(entt::id_type skill, int minLevel);

	entt::id_type getSkillId() const;
	int getLevelRequirement() const;

private:
	friend class cereal::access;
	template<typename Archive>
	void serialize(Archive& ar)
	{
		ar(_skillId, _minLevel);
	}

private:
	entt::id_type _skillId;
	int _minLevel = 0;
};