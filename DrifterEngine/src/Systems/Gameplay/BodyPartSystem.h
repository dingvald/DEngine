#pragma once
#include "Systems/System.h"

struct BodyPart;

namespace drft::system
{
	class BodyPartSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;

		void onMeleeAttackActionUpdated(entt::registry& registry, entt::entity entity);
	};

}


