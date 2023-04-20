#include "pch.h"
#include "DeathSystem.h"
#include "Factory/EntityFactory.h"
#include "Components/Components.h"
#include "Events/RequestStateChange.h"
#include "Engine/States/StateIdentifiers.h"
#include "Spatial/Conversions.h"
#include "Utility/EntityHelpers.h"

void drft::system::DeathSystem::init()
{
}

void drft::system::DeathSystem::update(const float dt)
{
	const auto& factory = registry->ctx().get<EntityFactory&>();
	auto view = registry->view<component::action::Die, component::Physical>();
	for (auto [entity, physical] : view.each())
	{
		auto& pos = registry->get<component::Position>(entity);
		for (auto matName : physical.materials)
		{
			auto dropped = factory.build(matName, *registry);
			dropped.patch<component::Position>([&pos](auto& position)
				{
					position.position = pos.position;
					position.depth = spatial::Layer::Item;
				});
		}
		if (registry->any_of<component::Player>(entity))
		{
			std::filesystem::remove_all(".\\data\\savegame\\");
			auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
			dispatcher.trigger(events::RequestStateStackPush(States::GameOver));
		}
		registry->destroy(entity);
	}
}
