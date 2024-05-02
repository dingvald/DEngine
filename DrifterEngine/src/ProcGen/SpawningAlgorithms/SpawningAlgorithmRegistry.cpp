#include "pch.h"
#include "SpawningAlgorithmRegistry.h"

const drft::gen::ISpawningAlgorithm& drft::gen::SpawningAlgorithmRegistry::get(const std::string& name) const
{
	if (!_spawningAlgorithms.contains(name)) throw std::exception("Registry does not contain algorithm");
	return *_spawningAlgorithms.at(name);
}

bool drft::gen::SpawningAlgorithmRegistry::contains(const std::string& name) const
{
	return _spawningAlgorithms.contains(name);
}
