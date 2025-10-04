#pragma once

namespace drft::math
{
	bool isNear(double d, double target);
	bool isNear(float f, float target);
	bool isNearZero(double d);
	bool isNearZero(float f);

	double lerp(double a, double b, double t);
	double inverseLerp(double a, double b, double v);
	double remap(double iMin, double iMax, double oMin, double oMax, double val);
	float floorToMultiple(float f, float multiple);
	float roundToMultiple(float f, float multiple);
	
	int wrap(int val, int min, int max);
	
	template<typename T>
	int sign(T val)
	{
		return (T(0) < val) - (val < T(0));
	}

	template<typename T>
	concept IsArithmetic = std::is_arithmetic<T>::value;

	template <IsArithmetic T>
	bool isOverlapping(T min1, T max1, T min2, T max2)
	{
		return max1 >= min2 && max2 >= min1;
	}

	template<IsArithmetic T>
	struct Range
	{
		Range() = default;
		Range(T min, T max)
			: _min(min)
			, _max(max) {}
		void setInfinite() 
		{ 
			_isInfinite = true;
			_min = std::numeric_limits<T>::min();
			_max = std::numeric_limits<T>::max();
		}
		bool isInfinite() const { return _isInfinite; }
		bool isValueWithin(T val) const
		{
			if (val > _min && val < _max) return true;
			return false;
		}
		bool isValueWithinInclusive(T val) const
		{
			if (val >= _min && val <= _max) return true;
			return false;
		}
		void setMax(T max) { _max = max; }
		void setMin(T min) { _min = min; }
		T getMax() const { return _max; }
		T getMin() const { return _min; }
		T distance(T val) const
		{
			if (isValueWithinInclusive(val)) return {};
			if (val < _min)
			{
				return std::abs(val - _min);
			}
			else
			{
				return std::abs(val - _max);
			}
		}
		T clamp(T val) const
		{
			return std::clamp(val, _min, _max);
		}
	private:
		T _min = std::numeric_limits<T>::max();
		T _max = std::numeric_limits<T>::min();
		bool _isInfinite = false;
	};
}