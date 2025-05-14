#pragma once

namespace drft
{
	enum class States : unsigned int
	{
		None,
		Title,
		MainMenu,
		Settings,
		Keybindings,
		Game,
		Inventory,
		Pause,
		GameOver,
		Crafting,
		SelectDirection,
		SelectTarget,
		Simulation,
		Skills
	};
}

