#pragma once
#include "StructureDecoration.h"

namespace drft
{
	class StructureInstance;
	class StructureDecoration;
	class StructureBaseShape;

	using BaseShapePtr = std::unique_ptr<StructureBaseShape>;
	using StructureInstancePtr = std::unique_ptr<StructureInstance>;

	class StructureBlueprint
	{
	private:
		friend class StructureFactory;
		StructureBlueprint(std::string name);
		void createFromJSON(const rapidjson::Value& json);
		StructureInstancePtr build() const;

	private:
		using DecorationPtr = std::unique_ptr<StructureDecoration>;
		std::string _name;
		BaseShapePtr _baseShape;
		std::vector<DecorationPtr> _decorations;
	};
}


