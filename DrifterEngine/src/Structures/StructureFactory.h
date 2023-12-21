#pragma once
#include "StructureBase.h"

namespace drft
{
	class StructureFactory
	{
	public:
		bool loadStructures(const std::string& JSONfilename);
		void initAllStructures();
		StructureBase& build(const std::string& structureName) const;

	private:
		std::unordered_map<std::string, std::unique_ptr<StructureBase>> _structures;
	};
}


