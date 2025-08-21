#pragma once

#include <AI/AiActions/IAiAction.h>

class AiActionRegistry
{
public:
	template<typename T>
	void registerAction(entt::id_type id)
	{
		_actions.emplace(id, std::make_unique<T>());
	}

	const IAiAction* getAction(entt::id_type id) const;

private:
	std::unordered_map<entt::id_type, std::unique_ptr<IAiAction>> _actions;
};