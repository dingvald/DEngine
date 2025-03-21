#include "pch.h"
#include "ContainerWrapper.h"
#include <Components/ContainerComponent.h>
#include <Systems/Helpers/ItemDatabase.h>
#include <Utility/EntityHelpers.h>
#include <Utility/StringManipulation.h>

void drft::ContainerWrapper::sort(SortType sortType)
{
	if (!isValid()) return;

	switch (sortType)
	{
	case drft::SortType::Alphabetic:
		std::sort(_component->contents.begin(), _component->contents.end(),
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

bool drft::ContainerWrapper::remove(entt::entity item)
{
	if (!isValid()) return false;

	if (auto itemComp = _registry->try_get<ItemComponent>(item))
	{
		size_t num = std::erase(_component->contents, itemComp->id);
		if (num > 0) return true;
	}
	return false;
}

bool drft::ContainerWrapper::add(entt::entity item)
{
	if (!isValid()) return false;
	if (_component->contents.size() >= _component->capacity) return false;

	if (auto itemComp = _registry->try_get<ItemComponent>(item))
	{
		_component->contents.push_back(itemComp->id);
		return true;
	}
	return false;
}

bool drft::ContainerWrapper::addBefore(entt::entity itemToAdd, size_t index)
{
	if (!isValid()) return false;
	if (_component->contents.size() >= _component->capacity) return false;
	if (index > _component->contents.size()) return add(itemToAdd);

	auto itemToAddComp = _registry->try_get<ItemComponent>(itemToAdd);

	if (!itemToAddComp) return false;

	_component->contents.insert(_component->contents.begin() + index, itemToAddComp->id);

	return true;
}

bool drft::ContainerWrapper::addAfter(entt::entity itemToAdd, size_t index)
{
	if (!isValid()) return false;
	if (_component->contents.size() >= _component->capacity) return false;

	return false;
}

const std::vector<ItemComponent::ID>& drft::ContainerWrapper::getItems() const
{
	if (!isValid())
	{
		throw std::exception("Underlying container is invalid");
	}
	return _component->contents;
}
