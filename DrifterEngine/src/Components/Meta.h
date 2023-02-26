#pragma once

#include "Components.h"

constexpr std::string_view INFO_STRING = "Info";
constexpr std::string_view POSITION_STRING = "Position";
constexpr std::string_view RENDER_STRING = "Render";
constexpr std::string_view ACTOR_STRING = "Actor";
constexpr std::string_view HEALTH_STRING = "Health";
constexpr std::string_view AI_STRING = "AI";

namespace component
{
	class Meta
	{
	public:
		static void initialize();
	};
}


