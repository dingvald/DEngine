#pragma once
#include <Generation/Features/IFeature.h>
#include <Utility/Math.h>

class ClusterFeature : public IFeature
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	TaggedPositions generateTags(sf::Vector3i position, const GenerationContext& context) const override;

private:
	std::vector<entt::id_type> _tags;
	drft::math::Range<float> _density;
	drft::math::Range<int> _radius;
};