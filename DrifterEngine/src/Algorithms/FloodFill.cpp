#include "pch.h"
#include "FloodFill.h"
#include "Utility/stdHashing.h"

std::vector<sf::Vector2i> drft::algo::floodFill(sf::Vector2i startingPosition, std::function<bool(sf::Vector2i)> isSameFunc)
{
	std::vector<sf::Vector2i> result;
	std::queue<sf::Vector2i> q;
	std::unordered_set<sf::Vector2i> visited;
	q.push(startingPosition);

	while (!q.empty())
	{
		sf::Vector2i current = q.front();
		q.pop();
		visited.insert(current);

		if (!isSameFunc(current)) continue;

		result.push_back(current);
		for (int y = -1; y <= 1; ++y)
		{
			for (int x = -1; x <= 1; ++x)
			{
				if (x == 0 && y == 0) continue;
				if (visited.contains(current + sf::Vector2i(x, y))) continue;
				q.push(current + sf::Vector2i(x, y));
				visited.insert(current + sf::Vector2i(x, y));
			}
		}
	}

	return result;
}
