#include "pch.h"
#include "BuildMany.h"
#include "Components/Components.h"
#include "Random/RandomNumberGenerator.h"
#include "Spatial/Conversions.h"
#include "Factory/EntityFactory.h"

void drft::util::buildMany(const std::string& entity, int number, sf::IntRect inArea, entt::registry& registry)
{
	auto& factory = registry.ctx().get<EntityFactory&>();

	for (int i = 0; i < number; ++i)
	{
		int x = rng::RandomNumberGenerator::intInRange(inArea.left, inArea.left + inArea.width - 1);
		int y = rng::RandomNumberGenerator::intInRange(inArea.top, inArea.top + inArea.height - 1);
		auto position = spatial::toWorldSpace({ x,y });

		factory.build(entity, registry)
			.patch<component::Position>([&position](auto& pos)
				{
					pos.position = position;
				});
	}
}
