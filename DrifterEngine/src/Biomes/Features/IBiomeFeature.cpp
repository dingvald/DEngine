#include "pch.h"
#include "IBiomeFeature.h"

FeatureGenerationResult IBiomeFeature::generate(const FeatureGenerationContext& context) const
{
	auto result = this->doGenerate(context);
	result.feature = this;
	return result;
}
