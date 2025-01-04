#pragma once

namespace drft::goap
{
	enum WorldStateType
	{
		target_x,
		target_y,
		action_counter,
		look_busy,
		visually_sense_hostile,
		spotted_hostile,
		kill_hostile,
		escape_hostile
	};
}