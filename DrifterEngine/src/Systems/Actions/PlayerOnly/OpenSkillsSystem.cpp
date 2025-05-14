#include "pch.h"
#include "OpenSkillsSystem.h"

#include <Components/Actions/OpenSkillsScreenAction.h>
#include <Components/SkillsComponent.h>
#include <Events/RequestStateChange.h>

void drft::system::OpenSkillsSystem::init()
{
	_registry.on_construct<OpenSkillsScreenAction>().connect<&OpenSkillsSystem::onOpenSkillsScreenAction>(this);
}

void drft::system::OpenSkillsSystem::onOpenSkillsScreenAction(entt::registry& registry, entt::entity entity) const
{
	if (!registry.all_of<SkillsComponent>(entity))
	{
		registry.remove<OpenSkillsScreenAction>(entity);
		return;
	}

	_dispatcher.trigger(events::RequestStateStackPush{ States::Skills });
}
