#include "pch.h"
#include "ContainerWrapper.h"
#include <Components/ContainerComponent.h>
#include <Systems/Helpers/ItemDatabase.h>
#include <Utility/EntityHelpers.h>
#include <Utility/StringManipulation.h>

void drft::ContainerWrapper::setContainer(entt::handle entity)
{
	if (auto container = entity.try_get<ContainerComponent>())
	{
		_container = container;
		_registry = entity.registry();
	}
}

bool drft::ContainerWrapper::isValid() const
{
	return _container != nullptr && _registry != nullptr;
}

void drft::ContainerWrapper::sort(SortType sortType)
{
	if (!isValid()) return;

	switch (sortType)
	{
	case drft::SortType::Alphabetic:
		std::sort(_container->contents.begin(), _container->contents.end(), 
			[this](ItemComponent::ID first, ItemComponent::ID second) {
				auto e1 = ItemDatabase::getEntityFromItemID(first);
				auto e2 = ItemDatabase::getEntityFromItemID(second);
				auto name1 = util::getEntityName({ *_registry, e1 });
				auto name2 = util::getEntityName({ *_registry, e2 });

				size_t i = 0;
				while (i < name1.length() && i < name2.length())
				{
					if (std::tolower(name1[i]) < std::tolower(name2[i])) return true;
					if (std::tolower(name1[i]) > std::tolower(name2[i])) return false;
					i++;
				}
				if (name1.length() < name2.length()) return true;
				return false;
			});
		break;
	default:
		break;
	}
}

const std::vector<ItemComponent::ID>& drft::ContainerWrapper::getItems() const
{
	if (!isValid())
	{
		throw std::exception("Underlying container is invalid");
	}
	return _container->contents;
}
