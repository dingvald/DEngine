#include "pch.h"
#include "FindItemOwner.h"
#include "Components/Components.h"

entt::entity drft::system::findItemOwner(entt::registry& registry, unsigned long itemID, WhereToLook whereToLook)
{
	switch (whereToLook)
	{
		case WhereToLook::Containers:
		{
			auto containerView = registry.view<component::Container>();
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
			auto bodyView = registry.view<component::Body>();
			for (auto [entity, body] : bodyView.each())
			{
				for (auto part : body.parts.flatten())
				{
					for (auto item : part->getAllSlotted())
					{
						if (item == itemID)
						{
							return entity;
						}
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
