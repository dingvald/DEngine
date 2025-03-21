#pragma once

namespace drft::events
{
	struct ChangeHUDEnabledEvent
	{
		std::optional<bool> shouldEnable = std::nullopt;
		std::optional<bool> shouldShow = std::nullopt;
	};
}
