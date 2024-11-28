#include "pch.h"
#include "FindItemOwner.h"
#include "Components/ContainerComponent.h"
#include "Components/BodyComponent.h"

entt::entity drft::system::findItemOwner(entt::registry& registry, unsigned long itemID, WhereToLook whereToLook)
{
	switch (whereToLook)
	{
		case WhereToLook::Containers:
		{
			auto containerView = registry.view<ContainerComponent>();
			for (auto [entity, container] : containerView.each())
			{
				for (auto item : container.contents)
				{
					if (item == itemID)
					{
						return entity;
					}
				}
			}
		}
		break;
		case WhereToLook::Bodies:
		{
			auto bodyView = registry.view<BodyComponent>();
			for (auto [entity, body] : bodyView.each())
			{
				for (auto item : body.parts.getAllEquipped())
				{
					if (item == itemID)
					{
						return entity;
					}
				}
			}

		}
		break;
		case WhereToLook::Both:
		{
			findItemOwner(registry, itemID, WhereToLook::Bodies);
			findItemOwner(registry, itemID, WhereToLook::Containers);
		}
		break;
	}

	return entt::null;
}
