#pragma once
#include "Spatial/Grid.h"
#include "Utility/stdHashing.h"

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

	struct BiomeType
	{
		std::string name;
		BiomeIcon icon;
		std::unordered_map<std::string, Range> ranges;
	};

	struct BiomeZone
	{
		BiomeZone(const BiomeType* type)
			:_type(type)
		{}
		const BiomeType* getType() const { return _type; }
		void setID(unsigned int id) { _id = id; }
		unsigned int getID() const { return _id; }
		int size() const { return _zone.size(); }
		void put(sf::Vector2i position) { _zone.insert(position); }
		const std::unordered_set<sf::Vector2i>& getZone() const { return _zone; }
	private:
		const BiomeType* _type;
		unsigned int _id = 0;
		std::unordered_set<sf::Vector2i> _zone;
	};

	class Biome
	{

		unsigned int zoneID = 0;
		spatial::Grid<int> reservedSpaces;
	};
}



