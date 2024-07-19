#pragma once
#include <ProcGen/ProbabilityMultipliers/ProbabilityMultiplier.h>

namespace drft::gen
{
	class NeighborMultiplier : public ProbabilityMultiplier
	{
	protected:
		void _createFromJSON(const rapidjson::Value& json) override;
		HitCount _test(sf::Vector2i position, GenerationContext& context) const override;

	private:
		entt::id_type _tag;
		int _max;
	};
}


