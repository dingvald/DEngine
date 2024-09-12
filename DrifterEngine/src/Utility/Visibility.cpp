#include "pch.h"
#include "Visibility.h"

Visibility::Visibility(std::function<bool(sf::Vector3i)> blocksLight, std::function<void(sf::Vector3i)> setVisible, std::function<int(sf::Vector3i)> getDistance)
	: _blocksLight(blocksLight)
	, _setVisible(setVisible)
	, _getDistance(getDistance)
{}

void Visibility::compute(sf::Vector3i origin, int radius)
{
	_setVisible(origin);
	for (unsigned int oct = 0; oct < 8; ++oct)
	{
		compute(oct, origin, radius, 1, Slope(1, 1), Slope(1, 0));
	}
}

void Visibility::compute(unsigned int octant, sf::Vector3i origin, int radius, unsigned int x, Slope top, Slope bottom) const
{
	for (; x < static_cast<unsigned int>(radius); ++x)
	{
		unsigned int topY;
		if (top.X == 1)
		{
			topY = x;
		}
		else
		{
			topY = ((x * 2 - 1) * top.Y + top.X) / (top.X * 2);
			if (blocksLight(x, topY, octant, origin))
			{
				if (top.greaterOrEqual(x * 2, topY * 2 + 1) && !blocksLight(x, topY + 1, octant, origin))
				{
					++topY;
				}
			}
			else
			{
				unsigned int ax = x * 2;
				if (blocksLight(x + 1, topY + 1, octant, origin)) ax++;
				if (top.greater(ax, topY * 2 + 1)) topY++;
			}
		}

        unsigned int bottomY;
        if (bottom.Y == 0)
        {
            bottomY = 0;
        }
        else
        {
            bottomY = ((x * 2 - 1) * bottom.Y + bottom.X) / (bottom.X * 2);
            if (bottom.greaterOrEqual(x * 2, bottomY * 2 + 1) && blocksLight(x, bottomY, octant, origin) &&
                !blocksLight(x, bottomY + 1, octant, origin))
            {
                bottomY++;
            }
        }

        int wasOpaque = -1;
        for (unsigned int y = topY; static_cast<int>(y) >= static_cast<int>(bottomY); --y)
        {
            if (radius < 0 || _getDistance(sf::Vector3i{ static_cast<int>(x),static_cast<int>(y),origin.z }) <= radius)
            {
                bool isOpaque = blocksLight(x, y, octant, origin);

                bool isVisible =
                    isOpaque || ((y != topY || top.greater(x * 4 + 1, y * 4 - 1)) && (y != bottomY || bottom.less(x * 4 - 1, y * 4 + 1)));

                if (isVisible) setVisible(x, y, octant, origin);

                if (x != radius)
                {
                    if (isOpaque)
                    {
                        if (wasOpaque == 0)
                        {
                            unsigned int nx = x * 2;
                            unsigned int ny = y * 2 + 1;

                            if (blocksLight(x, y + 1, octant, origin)) nx--; 
                            if (top.greater(nx, ny)) 
                            {                       
                                if (y == bottomY) 
                                { 
                                    bottom = Slope(nx, ny);
                                    break; 
                                } 
                                else compute(octant, origin, radius, x + 1, top, Slope(nx, ny));
                            }
                            else
                            {    
                                if (y == bottomY) return;
                            }
                        }
                        wasOpaque = 1;
                    }
                    else
                    {
                        if (wasOpaque > 0)
                        {
                            unsigned int nx = x * 2, ny = y * 2 + 1;
                            if (blocksLight(x + 1, y + 1, octant, origin)) nx++; 
                            if (bottom.greaterOrEqual(nx, ny)) return;
                            top = Slope(nx, ny);
                        }
                        wasOpaque = 0;
                    }
                }
            }
        }
        if (wasOpaque != 0) break;
    }
}

bool Visibility::blocksLight(unsigned int x, unsigned int y, unsigned int octant, sf::Vector3i origin) const
{
	int nx  = origin.x;
	int ny  = origin.y;
    int z   = origin.z;

	switch (octant)
	{
		case 0: nx += x; ny -= y; break;
		case 1: nx += y; ny -= x; break;
		case 2: nx -= y; ny -= x; break;
		case 3: nx -= x; ny -= y; break;
		case 4: nx -= x; ny += y; break;
		case 5: nx -= y; ny += x; break;
		case 6: nx += y; ny += x; break;
		case 7: nx += x; ny += y; break;
	}

	return _blocksLight(sf::Vector3i{ nx, ny, z });
}

void Visibility::setVisible(unsigned int x, unsigned int y, unsigned int octant, sf::Vector3i origin) const
{
	int nx  = origin.x;
	int ny  = origin.y;
    int z   = origin.z;

	switch (octant)
	{
	case 0: nx += x; ny -= y; break;
	case 1: nx += y; ny -= x; break;
	case 2: nx -= y; ny -= x; break;
	case 3: nx -= x; ny -= y; break;
	case 4: nx -= x; ny += y; break;
	case 5: nx -= y; ny += x; break;
	case 6: nx += y; ny += x; break;
	case 7: nx += x; ny += y; break;
	}

	_setVisible(sf::Vector3i{ nx, ny, z });
}
