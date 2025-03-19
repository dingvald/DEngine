#pragma once

namespace drft::system
{
	static constexpr unsigned int HOTBAR_SIZE = 10;

	// Converts a hotbar index to a vector index (0 - 9)
	size_t fromHotbarIndex(int index);
	// Converts a vector index to a hotbar index (1 - 0)
	int toHotbarIndex(size_t index);
}