#pragma once
#include <Biomes/Features/IBiomeFeature.h>
#include <Utility/Math.h>

namespace drft::spatial
{
	template<typename T>
	class Grid2d;
}
namespace drft::rng
{
	class Random;
}

class CaveBiomeFeature : public IBiomeFeature
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	FeatureGenerationResult doGenerate(const FeatureGenerationContext& context) const override;

private:
	using Grid = drft::spatial::Grid2d<bool>;
	void initGrid(Grid& grid, drft::rng::Random& random) const;
	void iteration(Grid& grid) const;
	int sumOfAdjacentWalls(int x, int y, int seachRadius, Grid& grid) const;

private:
	drft::math::Range<float> _radius;
	int _iterations = 1;
};