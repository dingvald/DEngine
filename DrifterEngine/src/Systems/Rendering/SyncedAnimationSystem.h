#pragma once
#include "Systems/System.h"
#include "Utility/stdHashing.h"

namespace drft::system
{
	class SyncedAnimationSystem : public System
	{
	public:
		using System::System;

	private:
		using IntDec = std::pair<int, int>;
		struct SyncPointData
		{
			float elapsed = 0.0f;
			unsigned int index = 0;
		};
	private:
		void update() override;
		void updateEnd() override;

		void updateSyncPoints();
		IntDec convertFloatToIntDec(float fl) const;
		float convertIntDecToFloat(IntDec intDec) const;

	private:
		std::vector<entt::entity> _toRemoveAnimation;
		std::unordered_map<IntDec, SyncPointData> _syncPoints;
	};
}


