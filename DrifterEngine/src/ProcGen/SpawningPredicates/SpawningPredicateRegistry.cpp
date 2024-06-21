#include "pch.h"
#include "SpawningPredicateRegistry.h"

const drft::gen::ISpawningPredicate& drft::gen::SpawningPredicateRegistry::get(const std::string& name) const
{
	if (!_spawningPredicates.contains(name)) throw std::exception("Registry does not contain predicate");
	return *_spawningPredicates.at(name);
}

bool drft::gen::SpawningPredicateRegistry::contains(const std::string& name) const
{
	return _spawningPredicates.contains(name);
}
