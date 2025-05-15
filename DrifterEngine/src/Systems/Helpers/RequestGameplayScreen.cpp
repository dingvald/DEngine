#include "pch.h"
#include "RequestGameplayScreen.h"

#include <Components/Actions/OpenAbilitiesScreenAction.h>
#include <Components/Actions/OpenSkillsScreenAction.h>
#include <Components/Components.h>
#include <Events/RequestStateChange.h>

#include <Utility/StandardLogger.h>

static const std::unordered_map<drft::States, std::function<void(entt::handle)>> StateComponentEmplaceMap = 
{
	{drft::States::Skills,		[](entt::handle entity) { entity.emplace<OpenSkillsScreenAction>(); }},
	{drft::States::Crafting,	[](entt::handle entity) { entity.emplace<component::action::OpenCrafting>(); }},
	{drft::States::Inventory,	[](entt::handle entity) { entity.emplace<component::action::OpenEquipment>(); }},
	{drft::States::Abilities,	[](entt::handle entity) { entity.emplace<OpenAbilitiesScreenAction>(); }},
};

void requestGameplayScreen(drft::States state, entt::handle entity)
{
	if (!StateComponentEmplaceMap.contains(state))
	{
		LOG_ERROR("Requesting state that does not require an entity");
		return;
	}
	if (!entity) return;
	if (!entity.registry()->ctx().contains<entt::dispatcher>()) return;

	StateComponentEmplaceMap.at(state)(entity);
	
	auto& dispatcher = entity.registry()->ctx().get<entt::dispatcher>();
	dispatcher.trigger(drft::events::RequestStateStackPush{ state });
}
