#pragma once
#include "GOAP/IAction.h"

namespace drft::goap
{
	class AttackHostileAction :
		public IAction
	{
	public:
		AttackHostileAction();
		// Inherited via IAction
		[[nodiscard]] virtual std::optional<sf::Vector2i> setMoveTarget(entt::const_handle agent) const override;
		virtual ActionResult perform(entt::handle agent) const override;
		virtual int cost() const override;
		virtual bool requiresInRange() const override;
		virtual bool isInRange(entt::const_handle agent) const override;
	};
}


