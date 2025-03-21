#pragma once

namespace drft::events
{
	struct ChangeMouseVisibilityEvent
	{
		bool showPath = true;
		bool showCursor = true;
	};
}