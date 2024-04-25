#pragma once
#include "Structures/StructureBase.h"

namespace drft
{
	class StructureRegistry
	{
	public:
		bool loadStructures(const std::string& JSONfilename);
		void resolveAllSubstructures();
		const StructureBase& lookup(const std::string& structureName) const;

	private:
		void resolve(StructureBase& toResolve, std::unordered_set<std::string>& resolved);

	private:
		std::unordered_map< std::string, std::unique_ptr<StructureBase> > _structures;
	};
}


