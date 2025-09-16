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
		bool isKeyboardControlled() const 
		{
			return _navigation == NavigationType::Keyboard;
		}
		bool isMouseControlled() const
		{
			return _navigation == NavigationType::Mouse;
		}

		void setNavigationType(NavigationType type)
		{
			_navigation = type;
		}
	private:
		NavigationType _navigation = NavigationType::Keyboard;
	};
}
