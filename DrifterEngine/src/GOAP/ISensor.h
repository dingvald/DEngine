#pragma once
#include "WorldState.h"
#include "SensorTypes.h"

namespace drft::goap
{
	struct SenseResult
	{
		bool keepSensing = false;
		bool success = false;
	};

	class ISensor
	{
	public:
		virtual SensorType getType() const = 0;
		// Senses an entity. Returns true is finished sensing (do no need to process all entities)
		virtual SenseResult sense(entt::handle agent, entt::entity surrounding = entt::null) const = 0;
		virtual WorldState getSenseSuccess() const = 0;
		virtual WorldState getSenseFailure() const = 0;
	};
}

