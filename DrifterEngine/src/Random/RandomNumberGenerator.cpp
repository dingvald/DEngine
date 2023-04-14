#include "pch.h"
#include "RandomNumberGenerator.h"

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

unsigned int drft::rng::generateSeed()
{
	return static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}
