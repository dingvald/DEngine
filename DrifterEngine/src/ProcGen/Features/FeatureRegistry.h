#pragma once
#include <ProcGen/Features/IFeature.h>
#include <Factory/Factory.h>
#include <ProcGen/Decorators/DecoratorFactory.h>

class FeatureRegistry
{
public:
	FeatureRegistry();

	void loadFeatures(const std::filesystem::path& featuresDirectory, const DecoratorFactory& decorators);
	const IFeature* get(entt::id_type id) const;

private:
	std::unordered_map<entt::id_type, IFeature::Ptr> _features;
	Factory<IFeature> _featureFactory;
};

