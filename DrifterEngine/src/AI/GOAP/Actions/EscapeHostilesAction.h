#pragma once
#include "AI/GOAP/IAction.h"

namespace drft::goap
{
	class EscapeHostilesAction :
		public IAction
	{
	public:
		EscapeHostilesAction();
		// Inherited via IAction
		virtual std::optional<TilePosition> trySetTarget(entt::handle agent) const override;
		virtual ActionResult perform(entt::handle agent) const override;
		virtual int cost() const override;
		virtual bool isInRange(entt::handle agent) const override;
	};
}


