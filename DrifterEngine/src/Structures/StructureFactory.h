#pragma once
#include "StructureBlueprint.h"

namespace drft
{
	class StructureInstance;

	class StructureFactory
	{
	public:
		void bindStructureShapes() const;
		void createStructureBlueprintsFromJSON(const std::filesystem::path& directoryPath);
		[[nodiscard]] StructureInstancePtr build(const std::string& name) const;

	private:
		std::unordered_map< std::string, StructureBlueprint > _blueprints;
	};
}


