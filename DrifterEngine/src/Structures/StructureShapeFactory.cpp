#include "pch.h"
#include "StructureShapeFactory.h"
#include "StructureBaseShape.h"

drft::StructureShapeFactory::FactoryMethodMap drft::StructureShapeFactory::_factoryMethods = {};

std::unique_ptr<drft::StructureBaseShape> drft::StructureShapeFactory::build(std::string name)
{
    if (!_factoryMethods.contains(name))
    {
        std::cout << "StructureShapeFactory does not contain component " << name << std::endl;
        return nullptr;
    }
    return std::move(_factoryMethods.at(name)());
}
