#include "pch.h"
#include "ContainerWrapper.h"
#include <Components/ContainerComponent.h>
#include <Systems/Helpers/ItemDatabase.h>
#include <Utility/EntityHelpers.h>
#include <Utility/StringManipulation.h>

static const std::vector<ItemComponent::ID> EmptyContainer = {};

void drft::ContainerWrapper::sort(SortType sortType)
{
	if (!isValid()) return;

	switch (sortType)
	{
	case drft::SortType::Alphabetic:
	{
		auto comp = tryGetUnderlying();
		auto registry = tryGetRegistry();
		std::sort(comp->contents.begin(), comp->contents.end(),
			[this, registry](ItemComponent::ID first, ItemComponent::ID second) {
				auto e1 = ItemDatabase::getEntityFromItemID(first);
				auto e2 = ItemDatabase::getEntityFromItemID(second);
				auto name1 = util::getEntityName({ *registry, e1 });
				auto name2 = util::getEntityName({ *registry, e2 });

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
	}
		break;
	default:
		break;
	}
}

bool drft::ContainerWrapper::remove(entt::entity item)
{
	if (!isValid()) return false;

	size_t result = 0;
	if (auto itemComp = tryGetRegistry()->try_get<ItemComponent>(item))
	{
		modify([&result, itemComp](ContainerComponent& comp) {result = std::erase(comp.contents, itemComp->id);});
	}
	return result > 0 ? true : false;
}

bool drft::ContainerWrapper::add(entt::entity item)
{
	if (!isValid()) return false;
	if (tryGetUnderlyingConst()->contents.size() >= tryGetUnderlyingConst()->capacity) return false;

	if (auto itemComp = tryGetRegistry()->try_get<ItemComponent>(item))
	{
		modify([id = itemComp->id](ContainerComponent& comp) {comp.contents.push_back(id);});
		return true;
	}
	return false;
}

bool drft::ContainerWrapper::addBefore(entt::entity itemToAdd, size_t index)
{
	if (!isValid()) return false;
	if (tryGetUnderlyingConst()->contents.size() >= tryGetUnderlyingConst()->capacity) return false;
	if (index > tryGetUnderlyingConst()->contents.size()) return add(itemToAdd);

	if (auto itemComp = tryGetRegistry()->try_get<ItemComponent>(itemToAdd))
	{
		modify([index, id = itemComp->id](ContainerComponent& comp) {comp.contents.insert(comp.contents.begin() + index, id);});
		return true;
	}

	return false;
}

bool drft::ContainerWrapper::addAfter(entt::entity itemToAdd, size_t index)
{
	if (!isValid()) return false;
	const size_t indexAfter = index + 1;
	if (indexAfter > tryGetUnderlyingConst()->contents.size()) return add(itemToAdd);

	if (auto itemComp = tryGetRegistry()->try_get<ItemComponent>(itemToAdd))
	{
		modify([indexAfter, id = itemComp->id](ContainerComponent& comp) {comp.contents.insert(comp.contents.begin() + indexAfter, id);});
		return true;
	}

	return false;
}

const std::vector<ItemComponent::ID>& drft::ContainerWrapper::getItems() const
{
	if (!isValid()) return EmptyContainer;
	return tryGetUnderlyingConst()->contents;
}
