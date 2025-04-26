#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class MaterialPropertiesSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;

		void onMaterialCompositionComponentAdded(entt::registry& registry, entt::entity);
		void onMaterialCompositionComponentUpdated(entt::registry& registry, entt::entity);
	};
}