#include "pch.h"
#include "DecoratorFactory.h"

#include <Generation/Decorators/EntityDecorator.h>
#include <Generation/Decorators/SimpleDecorator.h>
#include <Generation/Decorators/PrefabDecorator.h>

#include <Generation/Decorators/DecoratorIds.h>

void bindDecorators(DecoratorFactory& decoratorFactory)
{
	decoratorFactory.registerType<EntityDecorator>(DecoratorId::Entity);
	decoratorFactory.registerType<SimpleDecorator>(DecoratorId::Simple);
	decoratorFactory.registerType<PrefabDecorator>(DecoratorId::Prefab);
}
