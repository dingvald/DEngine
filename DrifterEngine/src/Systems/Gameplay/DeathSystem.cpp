#include "pch.h"
#include "DeathSystem.h"
#include "Factory/EntityFactory.h"
#include "Components/Components.h"
#include "Events/RequestStateChange.h"
#include "Engine/States/StateIdentifiers.h"
#include "Spatial/Conversions.h"
#include "Systems/Helpers/FindItemOwner.h"
#include "Random/RandomNumberGenerator.h"
#include "Utility/EntityHelpers.h"
#include "Events/ItemBreakEvent.h"
#include "Events/SendFloatingMessageEvent.h"

void drft::system::DeathSystem::init()
{
}

void drft::system::DeathSystem::update(const float dt)
{
	const auto& factory = _registry->ctx().get<EntityFactory&>();
	auto view = _registry->view<component::action::Die, component::Physical, component::Position>();
	for (auto [entity, physical, pos] : view.each())
	{
		int chance = 100;
		for (auto& matName : physical.materials)
		{
			int roll = rng::RandomNumberGenerator::intInRange(0, 100);
			if (roll <= chance)
			{
				auto dropped = factory.build(matName, *_registry);
				dropped.patch<component::Position>([&pos](auto& position)
					{
						position.position = pos.position;
					});
				if (matName.compare("Corpse") == 0)
				{
					auto entityName = util::getEntityName({ *_registry, entity });
					dropped.patch<component::Physical>([&physical](component::Physical& phy)
						{
							phy.weight = physical.weight;
						});
					dropped.patch<component::Info>([entityName, matName](component::Info& info)
						{
							info.name = entityName + "'s " + matName;
						});
				}
			}
			chance *= 0.5;
		}
		if (_registry->any_of<component::Player>(entity))
		{
			std::filesystem::remove_all(".\\data\\savegame\\");
			auto& dispatcher = _registry->ctx().get<entt::dispatcher&>();
			dispatcher.trigger(events::RequestStateStackPush(States::GameOver));
		}
		_registry->destroy(entity);
	}

	// Equipped item breaking
	auto& dispatcher = _registry->ctx().get<entt::dispatcher&>();
	auto itemView = _registry->view<component::action::Die, component::Item>(entt::exclude<component::Position>);
	for (auto [entity, item] : itemView.each())
	{
		auto owner = findItemOwner(*_registry, item.id, WhereToLook::Bodies);
		dispatcher.trigger(events::ItemBreakEvent(item.id, owner));
		dispatcher.trigger(events::SendFloatingMessageEvent{
			.message = util::getEntityName({*_registry, entity}) + " broke!",
			.color = sf::Color::Yellow,
			.tracksEntity = owner,
			.position = _registry->get<component::Position>(owner).position,
			.velocity = {0,-0.25},
			.isScreenSpace = false,
			.ttl = 100
			});
		_registry->destroy(entity);
	}
}
