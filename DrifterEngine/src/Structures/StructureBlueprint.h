#pragma once
#include "StructureDecorator.h"
#include "StructureBaseShape.h"

namespace drft
{
	class StructureInstance;

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
		std::string _name;
		BaseShapePtr _baseShape;
		std::vector<StructureDecorator> _decorators;
	};
}


