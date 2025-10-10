#pragma once
#include <Factory/Factory.h>
#include <Generation/Decorators/IDecorator.h>

using DecoratorFactory = Factory<IDecorator>;

void bindDecorators(DecoratorFactory& decoratorFactory);