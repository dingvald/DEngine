#include "pch.h"
#include "BodyPartSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Utility/ItemIDToEntityID.h"

void drft::system::BodyPartSystem::init()
{
}

void drft::system::BodyPartSystem::update(const float dt)
{
	auto view = registry->view<component::Body, component::action::LaunchAttack, component::tag::Active>();

	for (auto [entity, body, attack] : view.each())
	{
		if (body.parts.contains("HandR") && body.parts.at("HandR") != 0)
		{
			const auto rightHandItem = util::ItemIDToEntityID(body.parts.at("HandR"), *registry);
			float weight = 0.0f;
			if (auto physicalComp = registry->try_get<component::Physical>(rightHandItem))
			{
				weight += physicalComp->weight;
			}

			const float force = weight;
			attack.damage += static_cast<int>(std::ceil(force));
		}
	}
}
