#include "pch.h"
#include "ClusterFeature.h"
#include <Spatial/Helpers.h>
#include <Random/Random.h>
#include <ProcGen/GenerationContext.h>

void ClusterFeature::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("tags"))
	{
		for (auto&& tag : json["tags"].GetArray())
		{
			_tags.emplace_back(entt::hashed_string{ tag.GetString() });
		}	
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

TaggedPositions ClusterFeature::doGenerate(const GenerationContext& context) const
{
	TaggedPositions result;

	drft::rng::Random random{ context.seed };
	float radius = random.realInRange(_radius);
	float density = random.realInRange(_density);

	auto circle = drft::spatial::getIntCircleInRadius({ 0,0,0 }, radius);
	for (auto&& position : circle)
	{
		auto randomDouble = random.realInRange(0.0, 1.0);
		if (randomDouble < density) continue;
		
		for (auto&& tag : _tags)
		{
			result[tag].push_back(position);
		}
	}

	return result;
}
