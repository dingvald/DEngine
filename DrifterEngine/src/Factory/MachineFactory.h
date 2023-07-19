#pragma once
#include "Machines/Machine.h"
namespace drft
{
	class MachineFactory
	{
	public:
		template<typename T>
		void registerMachine(const std::string& machineName)
		{
			_factories[machineName] = []() {return Machine::Ptr(new T()); };
		}

		 Machine::Ptr build(const std::string& name) const;

	private:
		std::unordered_map<std::string, std::function<Machine::Ptr()>> _factories;
	};
}


