#pragma once

#include <AI/AiActions/IAiAction.h>

class AiActionRegistry
{
public:
	void registerAction(entt::id_type id, std::unique_ptr<IAiAction> action);

	const IAiAction& getAction(entt::id_type) const;

private:
	std::unordered_map<entt::id_type, std::unique_ptr<IAiAction>> _actions;
};