#pragma once

namespace drft::gen
{
	struct Range
	{
	public:
		Range(float min = 0.0, float max = 1.0)
			: _min(min)
			, _max(max)
		{}
		void setMax(float max) { _max = max; }
		void setMin(float min) { _min = min; }
		float getMax() const { return _max; }
		float getMin() const { return _min; }
		bool isWithinRange(float val) const
		{
			if (val < _max && val > _min)
				return true;
			else
				return false;
		}
		float distance(float val) const
		{
			if (isWithinRange(val)) return 0.0f;
			if (val < _min)
			{
				return std::abs(val - _min);
			}
			else
			{
				return std::abs(val - _max);
			}
		}
	protected:
		float _max = 1.0f;
		float _min = 0.0f;
	};

	struct BiomeIcon
	{
		unsigned int sprite = 4;
		sf::Color color = sf::Color::Magenta;
	};

	struct Biome
	{
		std::string name;
		BiomeIcon icon;
		Range temperature;
		Range humidity;
		Range altitude;
	};
}



