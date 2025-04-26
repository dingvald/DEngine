#include "pch.h"
#include "DamageTypes.h"

void Damage::mergeOrAdd(Damage::TypeMap& main, const Damage::TypeMap& other)
{
	for (auto&& [typeName, damage] : other)
	{
		main[typeName] += damage;
	}
}
