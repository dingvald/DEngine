#pragma once

namespace drft::math
{
	double lerp(double a, double b, double t);
	double inverseLerp(double a, double b, double v);
	double remap(double iMin, double iMax, double oMin, double oMax, double val);

	int wrap(int val, int min, int max);

	template<typename T>
	concept IsArithmetic = std::is_arithmetic<T>::value;

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
	private:
		T _min = std::numeric_limits<T>::max();
		T _max = std::numeric_limits<T>::min();
		bool _isInfinite = false;
	};
}