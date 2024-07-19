#pragma once
#include "ProbabilityMultiplier.h"

namespace drft::gen
{
	class NoiseLayerMultiplier : public ProbabilityMultiplier
	{
	protected:
		HitCount _test(sf::Vector2i position, GenerationContext& context) const override;
		void _createFromJSON(const rapidjson::Value& json) override;

	private:
		std::string _noiseName;
		std::function<bool(float, float)> _comparisonFunc;
		float _threshold;
	};
}


