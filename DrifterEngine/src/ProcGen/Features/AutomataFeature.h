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
	TaggedPositions generateTags(const GenerationContext& context) const override;

private:
	using Grid = drft::spatial::Grid2d<std::uint8_t>;
	void initGrid(Grid& grid, drft::rng::Random& random) const;
	void iteration(Grid& grid) const;
	int sumOfAdjacentCells(int x, int y, int seachRadius, Grid& grid) const;

private:
	TagList _onTags;
	TagList _offTags;
	TagList _centerTags;
	TagList _borderTags;

	drft::math::Range<int> _radius;
	int _iterations = 1;
};