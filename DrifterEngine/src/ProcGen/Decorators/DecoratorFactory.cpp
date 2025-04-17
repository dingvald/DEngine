#include "pch.h"
#include "DecoratorFactory.h"

#include <ProcGen/Decorators/EntityDecorator.h>
#include <ProcGen/Decorators/SimpleDecorator.h>

void bindDecorators(DecoratorFactory& decoratorFactory)
{
	decoratorFactory.registerType<EntityDecorator>("entity_decorator");
	decoratorFactory.registerType<SimpleDecorator>("simple_decorator");
}
