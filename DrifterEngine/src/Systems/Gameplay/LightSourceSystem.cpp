#include "pch.h"
#include "LightSourceSystem.h"

#include "Components/BodyComponent.h"
#include "Components/TemporaryLightSourceComponent.h"
#include "Components/LocalLightSourceComponent.h"
#include "Components/Tags.h"

#include "Systems/Helpers/ItemDatabase.h"


void drft::system::LightSourceSystem::update()
{
	_registry.clear<TemporaryLightSourceComponent>();

	auto bodyView = _registry.view<BodyComponent>();
	for (auto [entity, body] : bodyView.each())
	{
		auto equippedItems = body.parts.getAllEquipped();
		for (auto&& item : equippedItems)
		{
			const auto itemEntity = ItemDatabase::getEntityFromItemID(item);
			if (const auto light = _registry.try_get<LocalLightSourceComponent>(itemEntity))
			{
				if (auto tempLight = _registry.try_get<TemporaryLightSourceComponent>(entity))
				{
					std::uint8_t r = static_cast<std::uint8_t>(std::clamp(tempLight->color.r * (static_cast<float>(light->color.r) / 255.f), 0.f, 255.f));
					std::uint8_t g = static_cast<std::uint8_t>(std::clamp(tempLight->color.g * (static_cast<float>(light->color.g) / 255.f), 0.f, 255.f));
					std::uint8_t b = static_cast<std::uint8_t>(std::clamp(tempLight->color.b * (static_cast<float>(light->color.b) / 255.f), 0.f, 255.f));

					const float radius = std::max(tempLight->radius, light->radius);
					_registry.emplace_or_replace<TemporaryLightSourceComponent>(entity, radius, sf::Color(r, g, b, tempLight->color.a));
				}
				else
				{
					_registry.emplace_or_replace<TemporaryLightSourceComponent>(entity, light->radius, light->color);
				}
			}
		}
	}
}

