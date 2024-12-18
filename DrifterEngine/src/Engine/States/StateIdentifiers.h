#pragma once

namespace drft
{
	enum class States : unsigned int
	{
		None,
		Title,
		MainMenu,
		Game,
		Inventory,
		Pause,
		GameOver,
		Crafting,
		SelectDirection,
		SelectTarget,
		Simulation,
	};
}

