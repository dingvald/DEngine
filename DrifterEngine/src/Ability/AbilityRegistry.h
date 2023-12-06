#pragma once
#include "Ability/IAbility.h"
#include "Ability/AbilityType.h"

namespace drft
{
	class AbilityRegistry
	{
	public:
		static void bind();
		static const IAbility& get(AbilityType ability);

	private:
		using AbilityMap = std::unordered_map<AbilityType, std::unique_ptr<IAbility>>;
		static AbilityMap _abilities;
	};
}


