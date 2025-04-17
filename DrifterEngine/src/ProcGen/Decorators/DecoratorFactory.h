#pragma once
#include <Factory/Factory.h>
#include <ProcGen/Decorators/IDecorator.h>

using DecoratorFactory = Factory<IDecorator>;

void bindDecorators(DecoratorFactory& decoratorFactory);