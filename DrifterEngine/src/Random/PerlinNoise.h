#pragma once
namespace drft::rng
{
	class PerlinNoise
	{
	public:
		PerlinNoise(unsigned int seed = 0, int octaves = 8, float lacunarity = 2.0, float gain = 0.5);
		double gen(double x, double y, double z = 0.0) const;

	private:
		double _gen(double x, double y, double z) const;
		double _fade(double t) const;
		double _lerp(double t, double a, double b) const;
		double _grad(int hash, double x, double y, double z) const;

	private:
		unsigned int mSeed = 0;
		std::vector<int> mPermutations{ 256 };
		int mOctaves = 8;
		float mLacunarity = 2.0;
		float mGain = 0.5;
	};
}

