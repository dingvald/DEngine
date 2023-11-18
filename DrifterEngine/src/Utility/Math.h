#pragma once

namespace drft::math
{
	double lerp(double a, double b, double t);
	double inverseLerp(double a, double b, double v);
	double remap(double iMin, double iMax, double oMin, double oMax, double val);

	int wrap(int val, int min, int max);

	template<typename T>
	struct Range
	{
		Range(T min, T max)
			: min(min)
			, max(max) {}
		bool isValueWithin(T val) const
		{
			if (val > min && val < max) return true;
			return false;
		}
		bool isValueWithinInclusive(T val) const
		{
			if (val >= min && val <= max) return true;
			return false;
		}
		T min;
		T max;
	};
}