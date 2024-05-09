#include "pch.h"
#include "StructureComponentFactory.h"
#include "StructureComponent.h"

drft::StructureComponentFactory::FactoryMethodMap drft::StructureComponentFactory::_factoryMethods = {};

std::unique_ptr<drft::StructureComponent> drft::StructureComponentFactory::build(std::string name)
{
    if (!_factoryMethods.contains(name))
    {
        std::cout << "StructureComponentFactory does not contain component " << name << std::endl;
        return nullptr;
    }
    return std::move(_factoryMethods.at(name)());
}
