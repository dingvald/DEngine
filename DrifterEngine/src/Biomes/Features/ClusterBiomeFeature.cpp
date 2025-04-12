#include "pch.h"
#include "ClusterBiomeFeature.h"
#include <Spatial/Helpers.h>
#include <Random/Random.h>

void ClusterBiomeFeature::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("entity_slot"))
	{
		_entitySlot = entt::hashed_string{ json["entity_slot"].GetString() };
	}
	if (json.HasMember("density"))
	{
		_density.setMin(json["density"].GetArray()[0].GetFloat());
		_density.setMax(json["density"].GetArray()[1].GetFloat());
	}
	if (json.HasMember("radius"))
	{
		_radius.setMin(json["radius"].GetArray()[0].GetFloat());
		_radius.setMax(json["radius"].GetArray()[1].GetFloat());
	}
}

FeatureGenerationResult ClusterBiomeFeature::doGenerate(const FeatureGenerationContext& context) const
{
	FeatureGenerationResult result;

	drft::rng::Random random{ context.seed };
	float radius = random.realInRange(_radius);
	float density = random.realInRange(_density);

	auto circle = drft::spatial::getIntCircleInRadius({ 0,0,0 }, radius);
	

	for (auto&& position : circle)
	{
		auto randomDouble = random.realInRange(0.0, 1.0);
		if (randomDouble < density) continue;
		
		result.entityPositions.emplace_back(_entitySlot, position);
		result.area = drft::spatial::expandToFit(result.area, drft::spatial::toXY(position));
	}

	return result;
}
