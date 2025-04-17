#pragma once
#include <ProcGen/Features/IFeature.h>
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

class AutomataFeature : public IFeature
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	TaggedPositions doGenerate(const GenerationContext& context) const override;

private:
	using Grid = drft::spatial::Grid2d<std::uint8_t>;
	void initGrid(Grid& grid, drft::rng::Random& random) const;
	void iteration(Grid& grid) const;
	int sumOfAdjacentCells(int x, int y, int seachRadius, Grid& grid) const;

private:
	std::vector<entt::id_type> _tags;
	drft::math::Range<float> _radius;
	int _iterations = 1;
};