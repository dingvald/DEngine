#pragma once
#include <JSON/ICreateFromJson.h>

struct EntityPositionPair
{
	entt::id_type entity;
	sf::Vector3i position;
};

struct FeatureGenerationContext
{
	unsigned int seed = 0;
};

struct FeatureGenerationResult
{
	sf::IntRect area = { {INT_MAX, INT_MAX}, {INT_MIN, INT_MIN} };
	std::vector<EntityPositionPair> entityPositions;
};

class IBiomeFeature : public ICreateFromJson
{
public:
	using Ptr = std::unique_ptr<IBiomeFeature>;

	virtual void createFromJson(const rapidjson::Value& json) = 0;
	virtual FeatureGenerationResult generate(const FeatureGenerationContext& context) const = 0;
};