#pragma once

#include <AI/AiActions/IAiAction.h>

class MeleeAttackAiAction : public IAiAction
{
public:
	bool canPerform(entt::const_handle actor) const override;
	void perform(entt::const_handle actor, entt::const_handle target) const override;
	float range(entt::const_handle actor) const override;
};