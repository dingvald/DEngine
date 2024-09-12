#pragma once
class Visibility
{
public:
	Visibility(std::function<bool(sf::Vector3i)> blocksLight, std::function<void(sf::Vector3i)> setVisible, std::function<int(sf::Vector3i)> getDistance);
	void compute(sf::Vector3i origin, int radius);

private:
	struct Slope
	{
		Slope(unsigned int x, unsigned int y)
			: X(x), Y(y) {}
		Slope(const Slope&) = default;
		Slope& operator=(const Slope&) = default;
		Slope(Slope&&) = default;
		Slope& operator=(Slope&&) = default;

		bool greater(unsigned int x, unsigned int y) const { return Y * x > X * y; }
		bool greaterOrEqual(unsigned int x, unsigned int y) const { return Y * x >= X * y; }
		bool less(unsigned int x, unsigned int y) const { return Y * x < X* y; }

		unsigned int X;
		unsigned int Y;
	};

	void compute(unsigned int octant, sf::Vector3i origin, int radius, unsigned int x, Slope top, Slope bottom) const;
	bool blocksLight(unsigned int x, unsigned int y, unsigned int octant, sf::Vector3i origin) const;
	void setVisible(unsigned int x, unsigned int y, unsigned int octant, sf::Vector3i origin) const;

private:
	std::function<bool(sf::Vector3i)> _blocksLight;
	std::function<void(sf::Vector3i)> _setVisible;
	std::function<int(sf::Vector3i)> _getDistance;
};

