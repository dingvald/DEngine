#include "pch.h"
#include "StructureBaseShape.h"
#include "StructureShapeInstance.h"

std::unique_ptr<drft::StructureShapeInstance> drft::StructureBaseShape::generate()
{
	auto shape = std::make_unique<StructureShapeInstance>();
	this->doGenerate(*shape);
	return std::move(shape);
}
