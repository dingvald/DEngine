#include "pch.h"
#include "DecoratorFactory.h"

#include <ProcGen/Decorators/EntityDecorator.h>
#include <ProcGen/Decorators/SimpleDecorator.h>
#include <ProcGen/Decorators/PrefabDecorator.h>

#include <ProcGen/Decorators/DecoratorIds.h>

void bindDecorators(DecoratorFactory& decoratorFactory)
{
	decoratorFactory.registerType<EntityDecorator>(DecoratorId::Entity);
	decoratorFactory.registerType<SimpleDecorator>(DecoratorId::Simple);
	decoratorFactory.registerType<PrefabDecorator>(DecoratorId::Prefab);
}
