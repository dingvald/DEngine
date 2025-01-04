#pragma once
#include "AI/GOAP/IAction.h"

namespace drft::goap
{
	class InvestigateHostileAction :
		public IAction
	{
	public:
		InvestigateHostileAction();
		// Inherited via IAction
		virtual ActionResult perform(entt::handle agent) const override;
		virtual int cost() const override;
		virtual bool isInRange(entt::handle agent) const override;
	};
}


