#pragma once
#include "Ability/IAbility.h"

namespace drft
{
	class AbilityRegistry
	{
	public:
		static void bind();
		template<typename T>
		static void registerAbility(const char* name)
		{
			_abilities.emplace(entt::hashed_string{ name }, std::make_unique<T>(name));
		}
		static const IAbility& get(entt::id_type abilityId);

	private:
		using AbilityMap = std::unordered_map<entt::id_type, std::unique_ptr<IAbility>>;
		static AbilityMap _abilities;
	};
}


