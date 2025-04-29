#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class MaterialCompositionSystem : public System
	{
	public:
		using System::System;

	private:
		enum class MaterialType
		{
			Solid,
			Liquid,
			Gas
		};

		void init() override;

		void onMaterialCompositionComponentAdded(entt::registry& registry, entt::entity);
		void onMaterialCompositionComponentUpdated(entt::registry& registry, entt::entity);

		void mergeMaterialProperties(entt::handle subject, MaterialType primaryType, entt::const_handle other, float percent) const;
	};
}