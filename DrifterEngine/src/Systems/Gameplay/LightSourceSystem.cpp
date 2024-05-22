#include "pch.h"
#include "LightSourceSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Systems/Helpers/ItemDatabase.h"

void drft::system::LightSourceSystem::init()
{

}

void drft::system::LightSourceSystem::fixedUpdate()
{
	auto bodyView = _registry->view<component::Body, component::tag::InViewport>();
	for (auto [entity, body] : bodyView.each())
	{
		for (const auto part : body.parts.flatten())
		{
			for (auto item : part->getAllSlotted())
			{
				const auto itemEntity = ItemDatabase::getEntityFromItemID(item);
				if (const auto light = _registry->try_get<component::LightSource>(itemEntity))
				{
					if (auto tempLight = _registry->try_get<component::TempLightSource>(entity))
					{
						sf::Uint8 r = static_cast<sf::Uint8>(std::clamp(tempLight->color.r * (static_cast<float>(light->color.r) / 255.f), 0.f, 255.f));
						sf::Uint8 g = static_cast<sf::Uint8>(std::clamp(tempLight->color.g * (static_cast<float>(light->color.g) / 255.f), 0.f, 255.f));
						sf::Uint8 b = static_cast<sf::Uint8>(std::clamp(tempLight->color.b * (static_cast<float>(light->color.b) / 255.f), 0.f, 255.f));

						const float radius = std::max(tempLight->radius, light->radius);
						const float dropOff = std::min(tempLight->dropOff, light->dropOff);
						_registry->emplace_or_replace<component::TempLightSource>(entity, radius, dropOff, sf::Color(r, g, b, tempLight->color.a));
					}
					else
					{
						_registry->emplace_or_replace<component::TempLightSource>(entity, light->radius, light->dropOff, light->color);
					}
				}
			}
		}
	}
}

void drft::system::LightSourceSystem::onFixedUpdateEnd()
{
	_registry->clear<component::TempLightSource>();
}

