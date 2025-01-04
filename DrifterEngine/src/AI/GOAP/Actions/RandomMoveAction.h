#pragma once
#include "AI/GOAP/IAction.h"

namespace drft::goap
{
	class RandomMoveAction : public IAction
	{
	public:
		RandomMoveAction();
		// Inherited via IAction
		virtual ActionResult perform(entt::handle agent) const override;
		virtual int cost() const override;

		// Inherited via IAction
		virtual bool isInRange(entt::handle agent) const override;
	};
}

