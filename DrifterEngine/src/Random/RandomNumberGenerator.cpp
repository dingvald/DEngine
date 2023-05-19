#include "pch.h"
#include "RandomNumberGenerator.h"
#include "Spatial/Helpers.h"

using namespace drft::rng;

std::mt19937 RandomNumberGenerator::_rng(std::random_device{}());

void RandomNumberGenerator::setSeed(unsigned int seed)
{
	_seed = seed;
	_rng.seed(seed);
}

unsigned int drft::rng::RandomNumberGenerator::getSeed()
{ 
	return _seed;
}

double RandomNumberGenerator::realInRange(double min, double max)
{
	using Dist = std::uniform_real_distribution<double>;
	static Dist dist{};
	return dist(_rng, Dist::param_type(min, max));
}

int drft::rng::RandomNumberGenerator::intInRange(int min, int max)
{
	using Dist = std::uniform_int_distribution<int>;
	static Dist dist{};
	return dist(_rng, Dist::param_type(min, max));
}

sf::Vector2i drft::rng::RandomNumberGenerator::positionInCircle(sf::Vector2i origin, int radius)
{
	int x = intInRange(origin.x - radius, origin.x + radius);
	int y = intInRange(origin.y - radius, origin.y + radius);

	while (spatial::distance(origin, { x,y }) >= radius)
	{
		x = intInRange(origin.x - radius, origin.x + radius);
		y = intInRange(origin.y - radius, origin.y + radius);
	}

	return { x,y };
}

unsigned int drft::rng::generateSeed()
{
	return static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}
