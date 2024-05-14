#include "pch.h"
#include "StructureDecoratorFactory.h"
#include "StructureDecorator.h"

drft::StructureDecoratorFactory::FactoryMethodMap drft::StructureDecoratorFactory::_factoryMethods = {};

std::unique_ptr<drft::StructureDecorator> drft::StructureDecoratorFactory::build(std::string name)
{
    if (!_factoryMethods.contains(name))
    {
        std::cout << "StructureDecoratorFactory does not contain decorator " << name << std::endl;
        return nullptr;
    }
    return std::move(_factoryMethods.at(name)());
}
