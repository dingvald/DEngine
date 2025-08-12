#pragma once
#include "EnTT/entt.hpp"
#include <AI/Blackboard/Blackboard.h>

struct UtilityAIComponent
{
	entt::id_type archetype;
	Blackboard blackboard;
};