#pragma once
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

enum class CurrentActorState
{
	Pending,
	InProgress,
	Complete
};

struct CurrentActorComponent
{
	CurrentActorState state = CurrentActorState::Pending;
	int pointsSpent = 0;
	int ticks = 0;
};