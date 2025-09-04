#pragma once

#include <AI/AiActions/IAiAction.h>

class WaitAiAction : public IAiAction
{
public:
	bool isValid(entt::const_handle actor, OptionalTarget target) const override;
	void perform(entt::handle actor, OptionalTarget target) const override;
	bool isInRange(entt::const_handle actor, OptionalTarget target) const override;
};