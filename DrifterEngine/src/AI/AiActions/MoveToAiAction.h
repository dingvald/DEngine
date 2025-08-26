#pragma once

#include <AI/AiActions/IAiAction.h>

class MoveToAiAction : public IAiAction
{
public:
	bool canPerform(entt::const_handle actor) const override;
	void perform(entt::handle actor, entt::const_handle target) const override;
	bool isInRange(entt::const_handle actor, entt::const_handle target) const override;
};
