#pragma once
#include <Spatial/AutoGrid3d.h>
#include <Utility/stdHashing.h>
#include <Spatial/ChunkPosition.h>

class CanvasLayer
{
public:
	static const int DefaultPriority;
	static const int UninitializedPriority;

	// Sets a value in the canvas if the priority is greater or equal to the one already there
	void set(std::any val, sf::Vector3i position, int priority = DefaultPriority);
	std::any get(sf::Vector3i position) const;
	void discard(sf::Vector3i position);

private:
	void setOrReplace(std::any val, sf::Vector3i position, int priority);

private:
	struct PriorityValuePair
	{
		int priority = -1;
		std::any value;
	};
	drft::spatial::AutoGrid3d<PriorityValuePair> _grid{ drft::ChunkDimensions };
};

