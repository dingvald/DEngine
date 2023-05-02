#include "pch.h"
#include "PerlinNoise.h"
#include "RandomNoise.h"

using namespace drft::rng;

PerlinNoise::PerlinNoise(unsigned int seed, int octaves, float lacunarity, float gain)
	: mSeed(seed)
	, mOctaves(octaves)
	, mLacunarity(lacunarity)
	, mGain(gain)
{
	mPermutations.resize(256);
	std::iota(mPermutations.begin(), mPermutations.end(), 0);
	std::default_random_engine rng(seed);
	std::shuffle(mPermutations.begin(), mPermutations.end(), rng);
	mPermutations.insert(mPermutations.end(), mPermutations.begin(), mPermutations.end());
}

double PerlinNoise::gen(double x, double y, double z) const
{
	double amplitude = 1.0;
	double frequency = 1.0;
	double result = 0.0;
	double max = 0.0;

	for (int i = 0; i < mOctaves; ++i) {
		result += amplitude * _gen(x * frequency, y * frequency, z * frequency);
		max += amplitude;
		amplitude *= mGain;
		frequency *= mLacunarity;
	}
	return result / max;
}

double PerlinNoise::_gen(double x, double y, double z) const
{
	int X = static_cast<int>(floor(x)) & 255;
	int Y = static_cast<int>(floor(y)) & 255;
	int Z = static_cast<int>(floor(z)) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	double u = _fade(x);
	double v = _fade(y);
	double w = _fade(z);

	int A = mPermutations[X] + Y;
	int AA = mPermutations[A] + Z;
	int AB = mPermutations[A + 1] + Z;
	int B = mPermutations[X + 1] + Y;
	int BA = mPermutations[B] + Z;
	int BB = mPermutations[B + 1] + Z;

	double res = _lerp(w,
		_lerp(v,
			_lerp(u, _grad(mPermutations[AA], x, y, z), _grad(mPermutations[BA], x - 1, y, z)),
			_lerp(u, _grad(mPermutations[AB], x, y - 1, z), _grad(mPermutations[BB], x - 1, y - 1, z))
		),
		_lerp(v,
			_lerp(u, _grad(mPermutations[AA + 1], x, y, z - 1), _grad(mPermutations[BA + 1], x - 1, y, z - 1)),
			_lerp(u, _grad(mPermutations[AB + 1], x, y - 1, z - 1), _grad(mPermutations[BB + 1], x - 1, y - 1, z - 1))
		)
	);
	return (res + 1.0) / 2.0;
}

double PerlinNoise::_fade(double t) const
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::_lerp(double t, double a, double b) const
{
	return a + t * (b - a);
}

double PerlinNoise::_grad(int hash, double x, double y, double z) const
{
	int h = hash & 15;
	double u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
