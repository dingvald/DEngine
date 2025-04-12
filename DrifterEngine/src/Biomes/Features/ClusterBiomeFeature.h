#pragma once
#include <Biomes/Features/IBiomeFeature.h>
#include <Utility/Math.h>

class ClusterBiomeFeature : public IBiomeFeature
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	FeatureGenerationResult doGenerate(const FeatureGenerationContext& context) const override;

private:
	entt::id_type _entitySlot;
	drft::math::Range<float> _density;
	drft::math::Range<float> _radius;
};