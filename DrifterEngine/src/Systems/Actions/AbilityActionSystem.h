#pragma once

#include "Systems/System.h"

namespace drft
{
	class IAbility;
}

namespace drft::system
{
	class AbilityActionSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void update() override;

		void performAutoAbility(entt::handle actor, const IAbility& ability) const;
		void performTargetedAbility(entt::handle actor, const IAbility& ability) const;
		void performDirectionalAbility(entt::handle actor, const IAbility& ability) const;
	};
}