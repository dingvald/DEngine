#pragma once
#include "GOAP/IAction.h"

namespace drft::goap
{
	class InvestigateHostileAction :
		public IAction
	{
		InvestigateHostileAction();
		// Inherited via IAction
		virtual std::optional<sf::Vector2i> trySetTarget(entt::handle agent) const override;
		virtual ActionResult perform(entt::handle agent) const override;
		virtual int cost() const override;
		virtual bool isInRange(entt::handle agent) const override;
	};
}


