#include "pch.h"
#include "StructureDecorationFactory.h"
#include "StructureDecoration.h"

drft::StructureDecorationFactory::FactoryMethodMap drft::StructureDecorationFactory::_factoryMethods = {};

std::unique_ptr<drft::StructureDecoration> drft::StructureDecorationFactory::build(std::string name)
{
    if (!_factoryMethods.contains(name))
    {
        std::cout << "StructureDecorationFactory does not contain component " << name << std::endl;
        return nullptr;
    }
    return std::move(_factoryMethods.at(name)());
}
