#pragma once
#include <ProcGen/Features/IFeature.h>
#include <Utility/Math.h>

class ClusterFeature : public IFeature
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	TaggedPositions doGenerate(const GenerationContext& context) const override;

private:
	std::vector<entt::id_type> _tags;
	drft::math::Range<float> _density;
	drft::math::Range<float> _radius;
};