#pragma once

namespace drft
{
	enum class States : unsigned int
	{
		None,
		Title,
		MainMenu,
		Settings,
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

