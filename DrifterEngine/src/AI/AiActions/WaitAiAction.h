#pragma once

#include <AI/AiActions/IAiAction.h>

class WaitAiAction : public IAiAction
{
public:
	bool canPerform(entt::const_handle actor) const override;
	void perform(entt::handle actor, entt::const_handle target) const override;
	float range(entt::const_handle actor) const override;
};