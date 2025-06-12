#pragma once
#include <ProcGen/Features/IFeature.h>
#include <Utility/Math.h>

class SingleFeature : public IFeature
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	TaggedPositions generateTags(sf::Vector3i position, const GenerationContext& context) const override;

private:
	entt::id_type _tag;
};