#pragma once
#include <JSON/ICreateFromJson.h>

struct GenerationRegistries;

struct EntityPositionPair
{
	entt::id_type entity;
	sf::Vector3i position;
};

struct FeatureGenerationContext
{
	unsigned int seed = 0;
	const GenerationRegistries& registries;
};

class IBiomeFeature;

struct FeatureGenerationResult
{
	sf::IntRect area = { {INT_MAX, INT_MAX}, {INT_MIN, INT_MIN} };
	std::vector<EntityPositionPair> entityPositions;
	const IBiomeFeature* feature = nullptr;
};

class IBiomeFeature : public ICreateFromJson
{
public:
	using Ptr = std::unique_ptr<IBiomeFeature>;

	virtual void createFromJson(const rapidjson::Value& json) = 0;
	FeatureGenerationResult generate(const FeatureGenerationContext& context) const;


protected:
	virtual FeatureGenerationResult doGenerate(const FeatureGenerationContext& context) const = 0;
};