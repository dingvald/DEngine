#pragma once
#include "Machine/Machine.h"
namespace drft
{
	class MachineFactory
	{
	public:
		bool loadPrototypes(const std::string& JSONfilename);
		const Machine& build(const std::string& name) const;

	private:
		std::unordered_map<std::string, Machine> _machines;
	};
}


