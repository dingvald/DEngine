#include "pch.h"
#include "Random.h"
#include "Spatial/Helpers.h"

using namespace drft::rng;


drft::rng::Random::Random(unsigned int seed)
{
	_gen.seed(seed);
}

double Random::realInRange(double min, double max)
{
	using Dist = std::uniform_real_distribution<double>;
	Dist dist{};
	return dist(_gen, Dist::param_type(min, max));
}

int drft::rng::Random::intInRange(int min, int max)
{
	using Dist = std::uniform_int_distribution<int>;
	Dist dist{};
	return dist(_gen, Dist::param_type(min, max));
}

sf::Vector2i drft::rng::Random::positionInCircle(sf::Vector2i origin, int radius)
{
	int x = intInRange(origin.x - radius, origin.x + radius);
	int y = intInRange(origin.y - radius, origin.y + radius);

	while (drft::spatial::distance2d(origin, { x,y }) >= radius)
	{
		x = intInRange(origin.x - radius, origin.x + radius);
		y = intInRange(origin.y - radius, origin.y + radius);
	}

	return { x,y };
}

sf::Vector2i drft::rng::Random::positionInRect(sf::IntRect rect)
{
	int x = intInRange(rect.left, rect.left + rect.width);
	int y = intInRange(rect.top, rect.top + rect.height);

	return { x,y };
}

std::vector<sf::Vector2i> drft::rng::Random::shuffleRect(sf::IntRect rect)
{
	auto positions = spatial::getIntRect({ rect.left, rect.top }, rect.width, rect.height);
	std::shuffle(positions.begin(), positions.end(), _gen);
	return positions;
}

unsigned int drft::rng::generateSeed()
{
	return static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}
