#pragma once

namespace drft
{
	enum class NavigationType
	{
		Keyboard,
		Mouse
	};

	struct ControlsContext
	{
		NavigationType navigation = NavigationType::Keyboard;
	};
}
