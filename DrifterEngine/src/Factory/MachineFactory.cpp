#include "pch.h"
#include "MachineFactory.h"

drft::Machine::Ptr drft::MachineFactory::build(const std::string& name) const
{
	if (!_factories.contains(name)) return nullptr;

	return _factories.at(name)();
}
